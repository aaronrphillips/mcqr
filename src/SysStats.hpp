///////////////////////////////////////////////////////////////////////////////
// 
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
//  
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSSTAT_HPP
#define SYSSTAT_HPP

// System Includes
#include <string>
// External Includes
#include <sigar.h>
#include <sigar_format.h>
// Internal Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "SysStats"


using namespace std;




//==============================================================================
// Class: SysStats
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


class SysStats
{

public:

  // Getters/Setters

  inline double getKiB(void) const throw()
  {
    return(1024.0); 
  }


  inline double getMiB(void) const throw()
  {
    return(getKiB() * 1024.0); 
  }


  // Constructors

  SysStats(
            string const & p_host,
            double         p_max_load   =    1,
            double         p_max_memory = 1.0 * 1024.0 * 1024.0
          ) throw();


  // Constructors

  ~SysStats(void)
  {
    this->my_sigar = NULL;
    return;
  }


  // Encapsulated Behavior

  bool SysCanDoMoreWork(void) throw();
  bool CanFitBytes(u_int64_t bytes) throw();
  bool LoadLessThanMax(void) throw();
  bool MemLessThanMax(void)  throw();


private:

  // Constructors

  SysStats(void);
  SysStats(SysStats const & p_sysstat);


  // Operators

  SysStats & operator=( SysStats const & p_sysstat);
  bool       operator==(SysStats const & p_sysstat);
  bool       operator==(SysStats const & p_sysstat) const;
  bool       operator!=(SysStats const & p_sysstat) const;


  // Methods



  // Variables

  string    my_host;
  double    max_load;
  double    max_memory;

  //  NOTE:  Make sure to set to NULL in destructor
  //  NOTE:  Must be static (need to have a singleton)
  static sigar_t * my_sigar;    

};

#endif /* SYSSTAT_HPP */
