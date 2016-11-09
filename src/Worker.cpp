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
#include <fstream>
#include <functional>  // Needed for ref()
#include <cstdlib>
#include <unistd.h>
#include <thread>
// External Includes
#include <zmq.hpp>
// Internal Includes
// Application Includes
#include "Worker.hpp"
#include "SysStats.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Worker"


// Namespaces used
using namespace std;
using namespace std::chrono;


//==============================================================================
// Class Worker
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


Worker::Worker(
                std::shared_ptr<ProgramOptions>           p_program_options,
                string                            const & p_my_host,
                string                            const & p_worker_id
              ) throw() :
worker_id(p_worker_id),
my_host(p_my_host),
tmp_dir(p_program_options->getTempDir()),
ready(false)
{
  ready = true;
  log_stream.open(p_program_options->getLogDir() + "/" + worker_id + ".log");
  log_stream << "WORKER: " << my_host << ":  " << " Finished constructing Worker" << endl;
  return;
}


// ======================================================================
// Methods
// ======================================================================


void Worker::Start(zmq::context_t * context) throw()
{
  log_stream << "WORKER: " << my_host << ":  " << " Start()" << endl;
  sleep(10); // wait for controller to spin up

  zmq::socket_t worker(*context, ZMQ_REQ);
  assert(worker);

  try
  {
    //  Socket to receive messages (REQ/REP) on
    if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ", connecting to CONTROLLER REQ" << endl; }
    //  Configure socket to not wait at close time
    int linger = 0;
    worker.setsockopt (ZMQ_LINGER, &linger, sizeof (linger));
    worker.connect("inproc://5556");
    if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ", connected to CONTROLLER REQ" << endl; }

  }
  catch(zmq::error_t & e)
  {
    log_stream << "ERROR: WORKER: Can't connect to CONTROLLER REQ:" << e.what() << endl;
    return;
  }
  

  //  Socket to send results to
  zmq::socket_t sender(*context, ZMQ_PUSH);
  assert(sender);
  try 
  {
    if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ", connecting to CONTROLLER PUSH" << endl; } 
    sender.connect("inproc://5557");
    if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ", connected to CONTROLLER PUSH" << endl; }
  }
  catch(zmq::error_t & e)
  {
    log_stream << "ERROR: WORKER: Can't connect to sender:" << e.what() << endl;
    return;
  }

  //  Process messages from both sockets
  while (1)
  {
    vector<string> req_values;
    req_values.push_back("hostname");
    req_values.push_back(my_host);
    req_values.push_back("workerid");
    req_values.push_back(worker_id);
    req_values.push_back("active");
    req_values.push_back(ToString<int>(this->ready));  // if we didn't have enough memory to run, just request sleeps until we get a kill
    MessageWrapper req_wrapper(e_req_task, req_values);

    try
    {
      if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << this->worker_id << ", requesting seq from controller" << endl; } 
      worker.send(req_wrapper.getMessage());
    }
    catch(zmq::error_t & e)
    {
      log_stream << "ERROR: WORKER: Can't send request to controller:" << e.what() << endl;
      return;
    }

    zmq::message_t rep_message;
    try
    {
      if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << this->worker_id << ", waiting for REP seq from controller" << endl; }
      worker.recv(&rep_message);
      if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << this->worker_id << ", received REP seq from controller" << endl; } 
    }
    catch(zmq::error_t & e)
    {
      log_stream << "ERROR: WORKER: Can't recieve reply from controller:" << e.what() << endl;
      return;
    }
    

    MessageWrapper rep_wrapper(rep_message);
    switch (rep_wrapper.getMessageWrapperType())
    {
      case e_rep_task:
      {
        checkValues(rep_wrapper.getValues(), 10, "Worker task rep on " + my_host);
        u_int64_t uuid               = ToNumber<int>(rep_wrapper.getValues().at(1));
        string task                  = rep_wrapper.getValues().at(3);
        string create_time           = rep_wrapper.getValues().at(5);
        // There are placeholders for these, but we don't need to grab the dummy value that's in there now
        // 7 = receive time
        // 9 = finish time
        system_clock::time_point now = system_clock::now();
        time_t tt                    = ToNumber<time_t>(create_time);
        system_clock::time_point tc  = system_clock::from_time_t(tt);
        duration<double> time_span   = duration_cast<duration<double>>(now - tc);
    
        if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ":"  << rep_wrapper.getMessageWrapperType() << ":" << uuid << ": " << time_span.count() << " seconds" << endl; } 
    
        if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  UUID:" << uuid << " Task:" << task << endl; } 

        // run the task, and get the return code
        /*
        string code_file = tmp_dir + "/" + ToString<u_int64_t>(uuid) + ".status";
        task += " ; echo $? > " + code_file;
        */

        system(task.c_str());

        /*
        int ret_code = -1;
        ifstream in(code_file.c_str());
        string line;
        if (in.is_open())
        {
          getline(in, line);
          string trimmed = trim(line);
          if (trimmed.size() > 0) { ret_code = ToNumber<int>(trimmed); }
          in.close();
        }
        unlink(code_file.c_str());
        */
        
        if (getVerbose()) { log_stream << "WORKER: Completed task: UUID: " << uuid << endl; }

        system_clock::time_point after_process = system_clock::now();
        duration<double> proc_span   = duration_cast<duration<double>>(after_process - now);
    
        //  Send results to controller
        vector<string> done_values;
        done_values.push_back("hostname");
        done_values.push_back(my_host);
        done_values.push_back("workerid");
        done_values.push_back(worker_id);
        done_values.push_back("uuid");
        done_values.push_back(ToString<u_int64_t>(uuid));
        done_values.push_back("wait time");
        done_values.push_back(ToString<double>(time_span.count())); 
        done_values.push_back("proc time");
        done_values.push_back(ToString<double>(proc_span.count())); 
        MessageWrapper done_rep_wrapper(e_processed_task, done_values);
        try
        {
          if (getVerbose()) { log_stream << "WORKER: sending 'done' message" << endl; }
          sender.send(done_rep_wrapper.getMessage());
          if (getVerbose()) { log_stream << "WORKER: sent 'done' message" << endl; }
        }
        catch(zmq::error_t & e)
        {
          log_stream << "ERROR: WORKER: Can't send to sender:" << e.what() << endl;
          return;
        }
      }
      break;


      // handle kill 
      case e_rep_die:
      {
if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ": Received KILL from CONTROLLER" << endl; } 
        checkValues(rep_wrapper.getValues(), 2, "Worker die on " + my_host);
        sleep(ToNumber<int>(rep_wrapper.getValues().at(1)));
        return;
      }
      break;


      // handle wait
      case e_rep_wait:
      {
if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  " << worker_id << ": Received WAIT from CONTROLLER: " << ToNumber<int>(rep_wrapper.getValues().at(1)) << " seconds" << endl; }
        checkValues(rep_wrapper.getValues(), 2, "Worker sleep on " + my_host);
        sleep(ToNumber<int>(rep_wrapper.getValues().at(1)));
      }
      break;

      case e_req_task:
      case e_processed_task:
      default:
      break;

    }

    if (::g_interrupted) 
    {
      if (getVerbose()) { log_stream << "WORKER: " << my_host << ":  interrupt received, exiting worker." << endl; }
      break;
    }
  }

  //  Finished
  return;
}


//string Worker::print(void) const throw()
//{
//  return("NOT IMPLEMENTED");
//}


// -------------------------------------
// Friends
// -------------------------------------


//ostream & operator<<(ostream & p_os, Worker const * p_worker) throw()
//{
//  return(p_os << p_worker->print());
//}


//ostream & operator<<(ostream & p_os, Worker const & p_worker) throw()
//{
//  return(p_os << p_worker.print());
//}
