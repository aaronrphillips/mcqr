///////////////////////////////////////////////////////////////////////////////
// 
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////

#ifndef DEALER_HPP
#define DEALER_HPP

// System Includes
#include <map>
#include <string>
#include <queue>
#include <vector>
// External Includes
// Internal Includes
#include "ProgramOptions.hpp"
#include "MessageWrapper.hpp"
#include <zmq.hpp>


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Dealer"


using namespace std;


//==============================================================================
// Class: Dealer
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


class Dealer
{

public:

  // Getters/Setters


  // Constructors

  Dealer(
          std::shared_ptr<ProgramOptions>           p_program_options,
          string                            const & p_dealer_name,
          string                            const & p_my_host,
          vector<string>                            p_node_list
        ) throw();


  // Constructors

  ~Dealer(void)
  {
    return;
  }


  // Encapsulated Behavior

  int Start(zmq::context_t * context) throw();


private:


  // Constructors

  Dealer(void);
  Dealer(Dealer const & p_dealer);


  // Operators

  Dealer & operator=( Dealer const & p_dealer);
  bool     operator==(Dealer const & p_dealer);
  bool     operator==(Dealer const & p_dealer) const;
  bool     operator!=(Dealer const & p_dealer) const;


  // Variables

  string               out_file;
  string               dealer_name;
  string               my_host;
  map<u_int64_t, bool> uuids;
  map<string, bool>    workers;
  queue<string>        tasks;
  double               task_wait;
  string               progress_file;
  vector<string>       node_list;

};

#endif /* DEALER_HPP */
