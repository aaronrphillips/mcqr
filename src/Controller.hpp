///////////////////////////////////////////////////////////////////////////////
// 
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

// System Includes
#include <chrono>
#include <ctime>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <vector>
// External Includes
// Internal Includes
#include "MessageWrapper.hpp"
#include "ProgramOptions.hpp"
#include "Worker.hpp"
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Controller"


using namespace std;


//==============================================================================
// Class: Controller
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
//   None discovered
//
// KNOWN LIMITIATIONS:
//   None discovered
//
// SEE ALSO:
//------------------------------------------------------------------------------

// DEVELOPER DOCUMENTATION:   (those who develop/fix this class)
//   [put documentation here for developers that work on this code]
//==============================================================================


class Controller
{

public:

  // Getters/Setters


  // Constructors

  Controller(
          std::shared_ptr<ProgramOptions>           p_program_options,
          string                            const & p_dealer_name,
          string                            const & p_my_host,
          string                            const & p_controller_id
        ) throw();


  // Constructors

  ~Controller(void)
  {
    return;
  }


  // Encapsulated Behavior

  void Start(zmq::context_t * context) throw();


private:

  // Constructors

  Controller(void);
  Controller(Controller const & p_controller);

  // Methods
  bool okToLaunch();

  // Operators

  Controller & operator=( Controller const & p_controller);
  bool     operator==(Controller const & p_controller);
  bool     operator==(Controller const & p_controller) const;
  bool     operator!=(Controller const & p_controller) const;


  // Variables

  string                      controller_id;
  string                      dealer_name;
  string                      my_host;
  bool                        ready;
  int                         active_workers;
  int                         max_active_workers;
  double                      max_load;
  double                      max_memory;
  double                      task_wait;
  std::chrono::system_clock::time_point            last_run_time;
  int                         completed_tasks;
  int                         distributed_tasks;
  map<string, bool>           workers;
  int                         num_workers;
  vector<shared_ptr<Worker> > worker_vec;
  vector<shared_ptr<thread> > thread_vec;
  zmq::context_t *            worker_context;
  ofstream                    log_stream;

};

#endif /* CONTROLLER_HPP */
