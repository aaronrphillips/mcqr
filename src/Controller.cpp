///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2014.  PNNL.  All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have a_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <chrono>
#include <ctime>
#include <functional>  // Needed for ref()
#include <cstdlib>
#include <unistd.h>
// External Includes
#include <zmq.hpp>
// Internal Includes
// Application Includes
#include "Controller.hpp"
#include "SysStats.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Controller"


// Namespaces used
using namespace std;
using namespace std::chrono;


//==============================================================================
// Class Controller
//
// API Developer Documentation:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//------------------------------------------------------------------------------
// API User Documentation:  (those who use this class)
//
//   [put a single summary sentence describing this class here]
//
// SYNOPSIS:
//   [put an overview paragraph for this class here]
//
// DESCRIPTION:
//   [put a description section for this class here]
//
// FUTURE FEATURES:
//  None discovered
//
// KNOWN LIMITIATIONS:
//  None discovered
//
// SEE ALSO:
//==============================================================================


// ======================================================================
// Constructors
// ======================================================================


Controller::Controller(
                std::shared_ptr<ProgramOptions>           p_program_options,
                string                            const & p_dealer_name,
                string                            const & p_my_host,
                string                            const & p_controller_id
              ) throw() :
controller_id(p_controller_id),
dealer_name(p_dealer_name),
my_host(p_my_host),
ready(false),
active_workers(0),
max_active_workers(p_program_options->getMaxCoresPerNode()),
max_load(p_program_options->getMaxLoad()),
max_memory(p_program_options->getMaxMemory()),
task_wait(p_program_options->getTaskWait()),
last_run_time(system_clock::now()),
completed_tasks(0),
distributed_tasks(0),
worker_context(new zmq::context_t(1))
{
  log_stream.open(p_program_options->getLogDir() + "/" + controller_id + ".log");
  num_workers = max_active_workers == 0 ? thread::hardware_concurrency() : max_active_workers;
  int reserved_cpus = get_host_name() == trim_host_name(dealer_name) ? 2 : 1; // leave 1 for controller, 1 for dealer (if this is the dealer node)
  num_workers = num_workers <= (thread::hardware_concurrency() - reserved_cpus) ? num_workers : thread::hardware_concurrency() - reserved_cpus; // leave enough room for dealer/controller
  num_workers = num_workers > 0 ? num_workers : 1; // start at least 1 worker


  if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << "trying to build " << num_workers << " workers" << endl; }
  for (int idx = 0; idx < num_workers; ++idx)
  {
    if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << "trying to build worker " << idx << endl; }
    try
    {
      shared_ptr<Worker> worker(new Worker(p_program_options, my_host, my_host + "-" + ToString<int>(idx))); 
      worker_vec.push_back(worker);

      if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << "trying to start thread " << idx << endl; }
      shared_ptr<thread> tmp(new thread(&Worker::Start, &(*worker_vec.at(idx)), worker_context));
      thread_vec.push_back(tmp);
      if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << "started thread " << idx << endl; }
      tmp->detach();
    }
    catch(std::exception & e)
    {
      log_stream << "ERROR: Can't start worker thread:" << idx << ": " << e.what() << endl;
      return;
    }
  }
  if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << " done building workers" << endl; }
  return;
}


// ======================================================================
// Methods
// ======================================================================


void Controller::Start(zmq::context_t * context) throw()
{
  if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": Start()" << endl; }
  string protocol  = "inproc";
  string local_host  = "";
  string dealer_host = "";
  if (context == NULL)
  {
    context     = new zmq::context_t(1);
    protocol    = "tcp";
    dealer_host = dealer_name + ":";
    local_host  = "*:";
  }

  if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": connecting to DEALER using " << protocol << endl; }

  zmq::socket_t dealer_req(*context, ZMQ_REQ);
  assert(dealer_req);

  try
  {
    //  Socket to receive messages (REQ/REP) on
    if (getVerbose()) { log_stream << "CONTROLLER:" << controller_id << ":" << my_host << ", connecting to DEALER REQ:" << dealer_name << ":" << endl; }
    //  Configure socket to not wait at close time
    int linger = 0;
    dealer_req.setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
    dealer_req.connect ((protocol + "://" + dealer_host + "5555").c_str());
    if (getVerbose()) { log_stream << "CONTROLLER:" << controller_id << ":" << my_host << ", connected to DEALER REQ:" << dealer_name << ":" << endl; }

  }
  catch(zmq::error_t & e)
  {
    log_stream << "ERROR: CONTROLLER: Can't connect to DEALER REQ:" << e.what() << endl;
    return;
  }
  

  //  Socket to send results to
  zmq::socket_t dealer_push(*context, ZMQ_PUSH);
  assert(dealer_push);
  try 
  {
    if (getVerbose()) { log_stream << "CONTROLLER:" << controller_id << ":" << my_host << ", connecting to DEALER PUSH:" << dealer_name << ":" << endl; } 
    dealer_push.connect((protocol + "://" + dealer_host + "5558").c_str());
    if (getVerbose()) { log_stream << "CONTROLLER:" << controller_id << ":" << my_host << ", connected to DEALER PUSH:" << dealer_name << ":" << endl; }
  }
  catch(zmq::error_t & e)
  {
    log_stream << "ERROR: CONTROLLER: Can't connect to dealer_push:" << e.what() << endl;
    return;
  }

  //  Socket to receive messages from Workers
  zmq::socket_t  worker_pull(*worker_context, ZMQ_PULL);
  assert(worker_pull);
  try
  {
    if (getVerbose()) { log_stream << "CONTROLLER:" << my_host << ", binding to PULL:" << dealer_name << ":" << endl; }
    worker_pull.bind("inproc://5557");
    if (getVerbose()) { log_stream << "CONTROLLER:" << my_host << ", bound to PULL:" << dealer_name << ":" << endl; }
  }
  catch(zmq::error_t & e)
  {
    log_stream << "ERROR: CONTROLLER: can't connect to PULL worker_pull socket:" << e.what() << endl;
    return;
  }

  //  Socket for replying to Workers with a MessageWrapper message of type: sequence, wait, or die
  zmq::socket_t worker_rep(*worker_context, ZMQ_REP);
  assert(worker_rep);
  try
  {
    if (getVerbose()) { log_stream << "CONTROLLER:" << my_host << ", binding to REPLY:" << dealer_name << ":" << endl; }
    worker_rep.bind("inproc://5556");
    if (getVerbose()) { log_stream << "CONTROLLER:" << my_host << ", bound to REPLY:" << dealer_name << ":" << endl; }
  }
  catch(zmq::error_t & e)
  {
    log_stream << "ERROR: CONTROLLER: Can't bind to REPLY worker_rep socket:" << e.what() << endl;
    return;
  }


  //  Process messages from worker_rep and worker_pull
  vector<zmq::pollitem_t> items = {
                                     { worker_rep, 0, ZMQ_POLLIN, 0 },
                                     { worker_pull, 0, ZMQ_POLLIN, 0 }
                                  };

  int kills = 0;

  //  Process messages from all sockets
  while (1)
  {
    try
    {
      if (getVerbose()) { log_stream << "CONTROLLER:" << my_host << " waiting for worker socket" << endl; }
      zmq::poll(&items[0], items.size(), -1);
      if (getVerbose()) { log_stream << "CONTROLLER:" << my_host << " processing worker socket" << endl; }
    }
    catch(zmq::error_t & e)
    {
      log_stream << "ERROR: CONTROLLER: Can't poll worker_rep or worker_pull:" << e.what() << endl;
      return;
    }

    if (::g_interrupted)  
    {
      log_stream << "CONTROLLER: " << my_host << ": interrupt received, exiting Controller." << endl;
      break;
    }
  
    // process notification of completion
    if (items[1].revents & ZMQ_POLLIN)
    {
      try
      {
        zmq::message_t message;
        worker_pull.recv(&message);
        MessageWrapper wrapper(message);

        checkValues(wrapper.getValues(), 10, "Controller push on " + my_host);

        string hostname = wrapper.getValues().at(1);
        string workerid = wrapper.getValues().at(3);
        u_int64_t done_uuid  = ToNumber<u_int64_t>(wrapper.getValues().at(5));
        // 7 = wait time
        // 9 = proc time
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": got a PUSH from:" << hostname << ":" << workerid << " uuid:" << done_uuid << endl; }

        dealer_push.send(message);  // pass this along to the dealer
        active_workers--;
        completed_tasks++;
      }
      catch(zmq::error_t & e)
      {
        log_stream << "ERROR: CONTROLLER: Can't receive PUSH message on worker_rep:" << e.what() << endl;
        return;
      }
    }

    // process requests for work
    if (items[0].revents & ZMQ_POLLIN)
    {
      if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": got a REQUEST" << endl; }
      zmq::message_t message;
      worker_rep.recv(&message);
      MessageWrapper req_wrapper(message);

      checkValues(req_wrapper.getValues(), 6, "Controller req on " + my_host);

      string hostname = req_wrapper.getValues().at(1);
      string workerid = req_wrapper.getValues().at(3);
      bool   active   = req_wrapper.getValues().at(5) == "1" ? true : false; // see if this worker just wants a new sleep/kill

      if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": got a REQUEST from:" << hostname << ":" << workerid << " active?: " << active << endl; }

      vector<string> values = req_wrapper.getValues();
      values.push_back("controller_id");
      values.push_back(controller_id);
 
      if (! okToLaunch())
      {
        values[5] = "0"; // not enough memory, get either a sleep or kill from the server
      }
  
      MessageWrapper new_wrapper(e_req_task, values);

      workers[workerid] = true;

      // pass the request along to the dealer
      try 
      {
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << this->controller_id << ", requesting seq from dealer" << endl; }
        dealer_req.send(new_wrapper.getMessage());
      }
      catch(zmq::error_t & e)
      {
        log_stream << "ERROR: CONTROLLER: Can't send request to dealer:" << e.what() << endl;
        return;
      }
  
      // wait for the reply from the dealer
      zmq::message_t rep_message;
      try 
      {
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << this->controller_id << ", waiting for REP seq from dealer" << endl; }
        dealer_req.recv(&rep_message);
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": " << this->controller_id << ", received REP seq from dealer" << endl; }
      }
      catch(zmq::error_t & e)
      {
        log_stream << "ERROR: CONTROLLER: Can't recieve reply from dealer:" << e.what() << endl;
        return;
      }

      MessageWrapper rep_wrapper(rep_message);
      if (rep_wrapper.getMessageWrapperType() == e_rep_die)
      {
        kills++;
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": has now received " << kills << " kills (need " << num_workers << " to exit)" << endl; }
      }
      if (rep_wrapper.getMessageWrapperType() == e_rep_task)
      {
        active_workers++;
        distributed_tasks++;
        last_run_time = system_clock::now();
      }

      // pass the reply along to the worker
      try
      {
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": sending a message" << endl; }
        worker_rep.send(rep_message);
      }
      catch(zmq::error_t & e)
      {
        log_stream << "ERROR: CONTROLLER: can't send to worker:" << e.what() << endl;
        return;
      }

      if (kills == workers.size() && kills > 0 && workers.size() == num_workers)
      {
        if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": received all kills (" << kills << ")" << endl; }
        break;
      }
    }
  }
    
  sleep(10); // make sure the worker actually gets the last message
  if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": exiting" << endl; }

  //  Finished
  return;
}

bool Controller::okToLaunch()
{
  SysStats stats(my_host, max_load, max_memory);

  bool workers_ok = max_active_workers == 0 || active_workers < max_active_workers;
  bool load_ok    = max_load == 0 || stats.LoadLessThanMax();
  bool memory_ok  = max_memory == 0 || stats.MemLessThanMax();

  if (! (workers_ok && load_ok && memory_ok))
  {
    if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": WARNING: " << my_host << " not ok to launch: workers:" << workers_ok << " load:" << load_ok << " memory:" << memory_ok << endl; }
    return false;
  }

  system_clock::time_point now = system_clock::now();
  duration<double> time_span   = duration_cast<duration<double>>(now - last_run_time);
  if (distributed_tasks > 0 && time_span.count() < task_wait)
  {
    if (getVerbose()) { log_stream << "CONTROLLER: " << my_host << ": WARNING: " << my_host << " not ok to launch: (task wait not elapsed - " << time_span.count() << " >= " << task_wait << ")" << endl; }
    return false;
  }
  
  return true;
}

