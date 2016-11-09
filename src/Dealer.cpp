//////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2014.  PNNL.  All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have a_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


// System Includes
// External Includes
#include <chrono>
#include <ctime>
#include <fstream>
#include <queue>
#include <stdlib.h> 
#include <sstream> 
#include <thread>
#include <vector>
#include <unistd.h>
#include <zmq.hpp>
// Internal Includes
// Application Includes
#include "Dealer.hpp"
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Dealer"


// Namespaces used
using namespace std;
using namespace std::chrono;


//==============================================================================
// Class Dealer
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


Dealer::Dealer(
                std::shared_ptr<ProgramOptions>           p_program_options,
                string                            const & p_dealer_name,
                string                            const & p_my_host,
                vector<string>                            p_node_list
              ) throw() :
dealer_name(p_dealer_name),
my_host(p_my_host),
task_wait(p_program_options->getTaskWait()),
progress_file(p_program_options->getProgressFile()),
node_list(p_node_list)
{
  stringstream string_stream;
  string optional_str = (p_program_options->getProgressFile().empty() ? "" : " -p  " + p_program_options->getProgressFile() + " ") + (getVerbose() ? " -V " : "");
  string_stream << getExePath()
                << " -C " 
                << " -M " << get_host_name() 
                << " -c " << p_program_options->getMaxCoresPerNode() 
                << " -l " << p_program_options->getMaxLoad() 
                << " -m " << p_program_options->getMaxMemory() 
                << " -w " << p_program_options->getTaskWait() 
                << " -d " << p_program_options->getTempDir() 
                << " -L " << p_program_options->getLogDir() 
                << optional_str; 
  std::string mcqr_cmd = string_stream.str();
  if (getVerbose()) { cout << "Launching controller(s) with command: " << mcqr_cmd << endl; }

  for (auto node_name : node_list)
  {
    string exe = (get_host_name() == trim_host_name(node_name) || node_name == "localhost") ? "/bin/bash -c" : "ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no " + node_name;
    system((exe + " \"" + mcqr_cmd + " >/dev/null 2>/dev/null </dev/null  &\" ").c_str());
  }

  ifstream in(p_program_options->getTaskFile().c_str());
  string line;
  if (in.is_open())
  {
    while (getline(in, line))
    {
      string trimmed = trim(line);
      if (trimmed.size() > 0 && trimmed.at(0) != '#')
      {
        tasks.push(trimmed);
      }
    }
    in.close();
  }

  return;
}


// ======================================================================
// Methods
// ======================================================================


int getRandomSleepSecs(int min, int max)
{
  if (min == max) return min;
  return rand() % (max - min) + min;
}

int Dealer::Start(zmq::context_t * context) throw()
{
  string protocol  = "inproc";
  string host      = "";
  if (context == NULL)
  {
    context   = new zmq::context_t(1);
    protocol  = "tcp";
    host      = "*:";
  }
  if (getVerbose()) { cout << "DEALER: binding using " << protocol << endl; }

  //  Socket to receive messages from Workers
  zmq::socket_t  book_keeper(*context, ZMQ_PULL);
  assert(book_keeper);
  try
  {
    if (getVerbose()) { cout << "DEALER:" << my_host << ", binding to PULL:" << dealer_name << ":" << endl; }
    book_keeper.bind((protocol + "://" + host + "5558").c_str());
  }
  catch(zmq::error_t & e)
  {
    cerr << "ERROR: DEALER: can't connect to PULL book_keeper socket:" << e.what() << endl;
    return(-1);
  }


  //  Socket for replying to Workers with a MessageWrapper message of type: sequence, wait, or die
  zmq::socket_t server(*context, ZMQ_REP);
  assert(server);
  try
  {
    if (getVerbose()) { cout << "DEALER:" << my_host << ", binding to REPLY:" << dealer_name << ":" << endl; }
    //server.bind(string("tcp://" + dealer_name + ":5555").c_str());
    server.bind((protocol + "://" + host + "5555").c_str());
  }
  catch(zmq::error_t & e)
  {
    cerr << "ERROR: DEALER: Can't bind to REPLY server socket:" << e.what() << endl;
    return(-1);
  }


  u_int64_t uuid = 1;
  
  ofstream progress;
  if (! progress_file.empty())
  {
    progress.open(progress_file.c_str());
  }

  //  Process messages from server and book_keeper
  vector<zmq::pollitem_t> items = {
                                     { server,      0, ZMQ_POLLIN, 0 },
                                     { book_keeper, 0, ZMQ_POLLIN, 0 }
                                  };     
  if (getVerbose()) { cout << "DEALER:" << my_host << ", beginning poll"<< endl; }
  //  Process messages from both sockets 
  while (1)                           
  {
    try
    {   
      zmq::poll(&items[0], items.size(), -1);
    }   
    catch(zmq::error_t & e)
    {   
      cerr << "ERROR: DEALER: Can't poll server or book_keeper:" << e.what() << endl;
      return(-1);
    }     

    if (::g_interrupted)  
    {
      cerr << "DEALER: interrupt received, exiting dealer." << endl;
      break;
    }     

    if (items[1].revents & ZMQ_POLLIN)
    {
      try
      {
        zmq::message_t message;
        book_keeper.recv(&message);
        MessageWrapper wrapper(message);

        checkValues(wrapper.getValues(), 10, "Dealer push");

        string hostname = wrapper.getValues().at(1);
        string workerid = wrapper.getValues().at(3);
        u_int64_t done_uuid  = ToNumber<u_int64_t>(wrapper.getValues().at(5));
        double wait_secs = ToNumber<double>(wrapper.getValues().at(7));
        double proc_secs = ToNumber<double>(wrapper.getValues().at(9));

        if (getVerbose()) { cout << "DEALER: got a PUSH from:" << hostname << ":" << workerid << " uuid:" << done_uuid << endl; }
        if (progress.good()) { progress << "PROC TIME:" << done_uuid << "," << hostname << "," << workerid << "," << wait_secs << "," << proc_secs << ": seconds" << endl; }

        auto fitor = uuids.find(done_uuid);
        if (fitor != uuids.end())
        {
          if (fitor->second == false)
          {
            fitor->second = true;
          }
        }
        else
        {
          // TSNH
          cerr << "ERROR: DEALER: does not have an entry for uuid:" << done_uuid << endl;
        }
      }
      catch(zmq::error_t & e)
      {
        cerr << "ERROR: DEALER: Can't receive PUSH message on book_keeper:" << e.what() << endl;
        if (progress.good()) { progress.close(); }
        return(-1);
      }
    }


    if (items[0].revents & ZMQ_POLLIN)
    {
      if (getVerbose()) { cout << "DEALER: got a REQUEST" << endl; }
      zmq::message_t message;
      server.recv(&message);
      MessageWrapper req_wrapper(message);

      checkValues(req_wrapper.getValues(), 8, "Dealer req");

      string hostname      = req_wrapper.getValues().at(1);
      string workerid      = req_wrapper.getValues().at(3);
      bool   active        = req_wrapper.getValues().at(5) == "1" ? true : false; // see if this worker just wants a new sleep/kill
      string controller_id = req_wrapper.getValues().at(7);
      if (getVerbose()) { cout << "DEALER: got a REQUEST from:" << hostname << ":" << controller_id  << ":" << workerid << " active?: " << active << endl; }

      workers[hostname + "_" + workerid] = false;
      if (tasks.size() > 0 && active)
      {
        system_clock::time_point now          = system_clock::now();
        time_t tt                             = system_clock::to_time_t(now);
        string time_now                       = ToString<time_t>(tt);
      
        vector<string> values = {"id", "0", "task", "real_task", "create_time", "real_create_time", "receive_time", "real_receive_time", "finish_time", "real_finish_time"};

        uuids[uuid]                           = false;
        values.at(1) = ToString<int>(uuid);
        values.at(3) = tasks.front();
        tasks.pop();
        values.at(5) = time_now;
        values.at(7) = time_now;
        values.at(9) = time_now;
    
        MessageWrapper rep_wrapper(e_rep_task, values);
        try
        {
          if (getVerbose()) { cout << "DEALER: sending a message with UUID:" << uuid << endl; }
          server.send(rep_wrapper.getMessage());
        }
        catch(zmq::error_t & e)
        {
          cerr << "ERROR: DEALER: can't send to server:" << e.what() << endl;
          if (progress.good()) { progress.close(); }
          return(-1);
        }
        ++uuid;
      }
      else
      {
        u_int64_t uuids_processed = 0;
        u_int64_t total_uuids = uuids.size();
        string processing = "";
        for (auto uuid : uuids)
        {
          if (uuid.second == true) { uuids_processed++; }
          else 
          {
            processing += ToString<int>(uuid.first) + ", ";
          }
        }


        vector<string> values;
        wrapper_type_enum done_type;
        bool quit = true; 

        if (uuids_processed == total_uuids && uuids_processed > 0 && tasks.size() == 0) 
        {
          workers[hostname + "_" + workerid] = true;
          if (getVerbose()) { cout << "DEALER: sending KILL to:" << hostname << ":" << workerid << endl; }
          values = {"die", "1"};
          done_type = e_rep_die;

          for (auto itor : workers)
          {
            if (itor.second == false)
            {
              if (getVerbose()) { cout << "DEALER: can't exit - worker has not been killed:" << itor.first << endl; }
              quit = false;
              break;
            }
          }
        }
        else  // all sequences have been passed out, but not all are finished (or this is an inactive worker)
        {
          if (getVerbose() && active) { cout << "DEALER: still has " << (total_uuids - uuids_processed) << " queries out for processing - " << processing << endl; }
          if (getVerbose()) { cout << "DEALER: sending SLEEP to:" << hostname << ":" << workerid << endl; } 
          values = {"sleep", ToString<int>(getRandomSleepSecs(0, (int)task_wait)) };
          done_type = e_rep_wait;
          quit = false;
        }

        MessageWrapper done_wrapper(done_type, values);
        try
        {
          server.send(done_wrapper.getMessage());
        }
        catch(zmq::error_t & e)
        {
          cerr << "ERROR: DEALER: can't send " << values.at(0) << ":" << e.what() << endl;
          if (progress.good()) { progress.close(); }
          return(-1);
        }

        if (quit)
        {
          //return(0);
          if (getVerbose()) { cout << "DEALER: exiting" << endl; }
          break;
        }
      }
    }
  }

  sleep(10); // make sure the controller actually gets the last message
  if (progress.good()) { progress.close(); }

  return(0);
}


//string Dealer::print(void) const throw()
//{
//  return("NOT IMPLEMENTED");
//}


// -------------------------------------
// Friends
// -------------------------------------


//ostream & operator<<(ostream & p_os, Dealer const * p_dealer) throw()
//{
//  return(p_os << p_dealer->print());
//}


//ostream & operator<<(ostream & p_os, Dealer const & p_dealer) throw()
//{
//  return(p_os << p_dealer.print());
//}
