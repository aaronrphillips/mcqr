
#include <functional>  // Needed for ref()
#include <string>
#include <thread>
#include <time.h> 
#include <unistd.h>
#include <sys/types.h>

#include "global.hpp"
#include "Controller.hpp"
#include "Dealer.hpp"
#include "ProgramOptions.hpp"
#include "Worker.hpp"
#include <zmq.hpp>


using namespace std;


string dealer_name("localhost");
string my_host("localhost");


int main(int argc, char *argv[])
{
  srand (time(NULL));
  ::g_catch_signals();  

  setExeName(string(argv[0]));

  shared_ptr<ProgramOptions> po(new ProgramOptions());
  if (! po->checkOptions(argc, argv))
  {
    exit(1);
  }

  setVerbose(po->getVerbose());


  dealer_name = po->getMasterName();                   
  my_host = get_host_name();
  e_discovery_role my_role   = po->getRole();           
  string global_proc_id = get_host_name() + "-" + ToString<pid_t>(getpid()); 


  if (getVerbose()) 
  { 
    cout << "DEALER HOST:" << dealer_name     << ":" << endl; 
    cout << "MY     HOST:" << my_host         << ":" << endl;
    cout << "MY     ROLE:" << my_role         << ":" << endl;
  }

  if (e_controller == my_role)
  {
    try
    {
      if (getVerbose()) { cout << "IM A CONTROLLER: " << my_host << " : " << endl; }
      sleep(5);
      Controller controller(po, dealer_name, my_host, global_proc_id);
      controller.Start(NULL);
      if (getVerbose()) { cout << "controller Start() exited: " << my_host << " : " << endl; }
    }
    catch (...)
    {
      cerr << "CAUGHT ERROR FROM CONTROLLER" << endl;
    }
  }
  if (e_dealer == my_role)
  {
    try
    {
      if (getVerbose()) { cout << "IM A DEALER: " << my_host << endl; }
      Dealer dealer(po, dealer_name, my_host, po->getHostList());
      dealer.Start(NULL);
    }
    catch (...)
    {
      cerr << "CAUGHT ERROR FROM DEALER" << endl;
    }
  }
  if (e_worker == my_role)
  {
    try
    {
      if (getVerbose()) { cout << "IM A WORKER: " << my_host << " : " << endl; }
      //sleep(10); // Dealer does this in Start()
      Worker worker(po, my_host, global_proc_id);
      worker.Start(NULL);
    }
    catch (...)
    {
      cerr << "CAUGHT ERROR FROM WORKER" << endl;
    }
  }
  if (e_all == my_role)
  {
    vector<shared_ptr<Worker> > worker_vec;
    vector<shared_ptr<thread> > thread_vec;
    int threads = po->getMaxCoresPerNode();
    if (threads == 0) { threads = thread::hardware_concurrency() - 2; } // 0 = max possible
    if (threads <  1) { threads = 1; }                                  // need at least 1

    zmq::context_t * c_ptr = new zmq::context_t(1);
    Dealer dealer(po, dealer_name, my_host, po->getHostList());
    thread dealer_thread(&Dealer::Start, &(dealer), c_ptr);
    sleep(2);
    Controller controller(po, dealer_name, my_host, global_proc_id);
    thread controller_thread(&Controller::Start, &(controller), c_ptr);
    sleep(2);

    for (int idx = 0; idx < threads; ++idx)
    {
      try
      {
        if (getVerbose()) { cout << "trying to build worker" << endl; }
        shared_ptr<Worker> tmp_worker(new Worker(po, my_host, global_proc_id)); // 2 because of the dealer and controller
        worker_vec.push_back(tmp_worker);
  
        if (getVerbose()) { cout << "trying to start thread " << idx << endl; }
        shared_ptr<thread> tmp(new thread(&Worker::Start, &(*worker_vec.at(idx)), c_ptr));
        thread_vec.push_back(tmp);
      }
      catch(std::exception & e)
      {
        cerr << "ERROR: Can't start worker thread:" << idx << ": " << e.what() << endl;
        return(-1);
      }
    }

    
    if (getVerbose()) { cout << "MCQUEUER DOING WORK" << endl; }

    //  Join pool of worker threads
    for (int idx = 0; idx != thread_vec.size(); ++idx)
    {
      try
      {
        thread_vec[idx]->join();
        if (getVerbose()) { cout << "WORKER: Joined thread:" << idx << ":" << endl; }
      }
      catch(std::exception & e)
      {
        cerr << "ERROR: Can't join worker thread:" << idx << ": " << e.what() << endl;
        return(-1);
      }
    }
    if (getVerbose()) { cout << "MCQUEUER worker_threads joined" << endl; }

    controller_thread.join();
    if (getVerbose()) { cout << "MCQUEUER controller_thread joined" << endl; }

    dealer_thread.join();
    if (getVerbose()) { cout << "MCQUEUER dealer_thread joined" << endl; }

  }
  if (getVerbose()) { cout << "MCQUEUER DONE" << endl; }

  return(0);
}
