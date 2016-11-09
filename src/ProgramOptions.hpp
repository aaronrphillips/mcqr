///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  .  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_hdr.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP


// System Includes
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "ProgramOptions"


// Namespaces used
using namespace std;


//==============================================================================
// Class ProgramOptions
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


class ProgramOptions
{

public:

  // Getters/Setters


  inline bool getVerbose(void) throw()
  {
    return(this->verbose);
  }

  inline string getTaskFile(void) throw()
  {
    return(this->task_file);
  }

  inline string getProgressFile(void) throw()
  {
    return(this->progress_file);
  }

  inline int getMaxCoresPerNode(void) throw()
  {
    return(this->max_cores);
  }

  inline double getMaxLoad(void) throw()
  {
    return(this->max_load);
  }

  inline double getMaxMemory(void) throw()
  {
    return(this->max_memory);
  }

  inline double getTaskWait(void) throw()
  {
    return(this->task_wait);
  }

  inline string getTempDir(void) throw()
  {
    return(this->tmp_dir);
  }

  inline string getMasterName(void) throw()
  {
    return master_name;
  }

  inline e_discovery_role getRole(void) throw()
  {
    return role;
  }

  inline vector<string> getHostList(void) throw()
  {
    return host_list;
  }

  inline string getLogDir(void) throw()
  {
    return log_dir;
  }

  // Constructors

  ProgramOptions(void) throw();


  // Destructor

  ~ProgramOptions(void) throw()
  {
    return;
  }


  // Public Functions

  bool   checkOptions(int argc, char ** argv) throw();


private:

  // Getters/Setters

  template<class T>
  T getOption(string const & p_option)
  {
    T value;
    istringstream ss(p_option);
    try
    {
      ss >> value;
      return(value);
    }
    catch (const std::exception& e)
    {
      cerr << "ERROR: " << e.what() << endl;
      //FATAL(BadOption, "Processing options", p_option);
      return(*(T*)0);
    }
  }

  // Constructors

  ProgramOptions(ProgramOptions const & p_program_options) throw();


  // Operators

  ProgramOptions &  operator=(ProgramOptions const & p_program_options) throw();
  bool              operator==(ProgramOptions const & p_program_options) const throw();
  bool              operator!=(ProgramOptions const & p_program_options) const throw();


  // Variables

  bool              verbose;
  string            task_file;
  int               max_cores;
  double            max_cpu;
  double            max_load;
  double            max_memory;
  double            task_wait;
  string            progress_file;
  string            tmp_dir;
  string            master_name;
  e_discovery_role  role;
  vector<string>    host_list;
  string            log_dir;
  struct option     long_options[100];

  // Private Functions

  void displayHelp(bool const p_condition) throw();
  void displayVersion(bool const p_condition) throw();

  void checkEnum(string const & p_option, int unsigned p_value, int unsigned const p_check0, int unsigned const p_check1);
  void checkGreater(string const & p_option0, string const & p_option1);
  void checkRange(string const & p_option, int unsigned p_value, u_int64_t const p_check0, u_int64_t const p_check1);
  void checkRelation(string const & p_option0, string const & p_option1);
  void checkSpecial(string const & p_option, u_int64_t const p_check0, u_int64_t const p_check1);
  void conflictingOptions(string const & p_opt1, string const & p_opt2) throw(logic_error);
  void optionDependency(string const & p_for_what, string const & p_required_option) throw(logic_error);
  void fileMustExist(string const & p_file, int const p_flag) throw(logic_error);
  void fileMustNotExist(string const & p_file) throw(logic_error);
  void requiredFile(string const & p_file) throw(logic_error);
  void requiredOption(string const & p_option, string p_value) throw(logic_error);
  void requiredOptions(string const & p_opt1, string const & p_opt2) throw(logic_error);


};

#endif // PROGRAMOPTIONS_HPP
