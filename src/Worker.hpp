///////////////////////////////////////////////////////////////////////////////
// 
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////

#ifndef WORKER_HPP
#define WORKER_HPP

// System Includes
#include <map>
#include <string>
#include <vector>
#include <fstream>
// External Includes
// Internal Includes
#include "MessageWrapper.hpp"
#include "ProgramOptions.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Worker"


using namespace std;


//==============================================================================
// Class: Worker
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


class Worker
{

public:

  // Getters/Setters


  // Constructors

  Worker(
          std::shared_ptr<ProgramOptions>           p_program_options,
          string                            const & p_my_host,
          string                            const & p_worker_id
        ) throw();


  // Constructors

  ~Worker(void)
  {
    return;
  }


  // Encapsulated Behavior

  void Start(zmq::context_t * context) throw();


private:

  // Constructors

  Worker(void);
  Worker(Worker const & p_worker);


  // Operators

  Worker & operator=( Worker const & p_worker);
  bool     operator==(Worker const & p_worker);
  bool     operator==(Worker const & p_worker) const;
  bool     operator!=(Worker const & p_worker) const;


  // Variables

  string   worker_id;
  string   my_host;
  string   tmp_dir;
  bool     ready;
  ofstream log_stream;

};

#endif /* WORKER_HPP */
