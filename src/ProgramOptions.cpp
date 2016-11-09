///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  .  All Rights Reserved. 
//
// THIS FILE INITIALLY CREATED WITH:  
//     TEMPLATE NAME:  lang_cpp_class.template 
//     COMMAND NAME:   gensrc 
//
// CODING CONVENTIONS: 
//    * Class names are CamelCase with first word upper case 
//    * Functions are camelCase with first word lower case 
//    * Function parameters are lower case with _ and have p_ prefix 
//    * Member variables always use 'this' pointer 
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>
// External Includes
// Internal Includes
// Application Includes
#include "Exception.hpp"
#include "global.hpp"
#include "osUtil.hpp"
#include "ProgramOptions.hpp"


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


// ======================================================================
// Constructors
// ======================================================================


ProgramOptions::ProgramOptions(void) throw() :
verbose(false),
max_cores(0),
max_load(0),
max_memory(0),
task_wait(15.0),
progress_file(""),
tmp_dir("/tmp"),
master_name("localhost"),
role(e_dealer),
host_list(vector<string>()),
log_dir("."),
long_options({
  {"set-master", required_argument,       0, 'M'},
  {"worker",           no_argument,       0, 'W'},
  {"controller",       no_argument,       0, 'C'},
  {"host-list",  optional_argument,       0, 'H'},
  {"log-dir",    optional_argument,       0, 'L'},
  {"help",             no_argument,       0, 'h'},
  {"verbose",          no_argument,       0, 'V'},
  {"version",          no_argument,       0, 'v'},
  {"tasks",      required_argument,       0, 't'},
  {"max-cores",  optional_argument,       0, 'c'},
  {"max-load",   optional_argument,       0, 'l'},
  {"max-memory", optional_argument,       0, 'm'},
  {"task-wait",  optional_argument,       0, 'w'},
  {"prog-file",  optional_argument,       0, 'p'},
  {"temp-dir",   optional_argument,       0, 'd'},
  {0, 0, 0, 0}
})
{
  return;
}


bool ProgramOptions::checkOptions(int p_argc, char ** p_argv) throw()
{
  int opt;
  bool help = 0;
  bool version = 0;
  while (1)
  {
    /* getopt_long stores the option index here. */
    int option_index = 0;
     
    opt = getopt_long(p_argc, p_argv, "WChvVt:c:u:l:m:w:p:d:M:H:L:", long_options, &option_index);
     
    /* Detect the end of the options. */
    if (opt == -1)
    {
      break;
    }
     
    switch (opt)
    {
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
          break;
        if (optarg)
        break;
     
      case 'M':
        master_name = optarg;
        break;
     
      case 'L':
        char real[PATH_MAX];
        realpath(optarg, real);
        log_dir = string(real);
        break;
     
      case 'W':
        role = e_worker;
        break;
     
      case 'C':
        role = e_controller;
        break;
     
      case 'H':
        host_list = split(optarg, ',');
        break;
     
      case 'V':
        verbose = true;
        break;
     
      case 'h':
        help = true;
        break;
     
      case 'v':
        version = true;
        break;
     
      case 'w':
        task_wait = ToNumber<double>(optarg);
        break;
     
      case 'c':
        max_cores = ToNumber<int>(optarg);
        break;
     
      case 'l':
        max_load = ToNumber<double>(optarg);
        break;
     
      case 'm':
        max_memory = ToNumber<double>(optarg);
        break;
     
      case 't':
        char real1[PATH_MAX];
        realpath(optarg, real1);
        task_file = string(real1);
        break;
     
      case 'p':
        char real2[PATH_MAX];
        realpath(optarg, real2);
        progress_file = string(real2);
        break;
     
      case 'd':
        tmp_dir = optarg;
        break;
     
      case '?':
        /* getopt_long already printed an error message. */
        break;
     
      default:
        abort ();
    }
  }
     

  try
  {
    // Handle command line only options

    displayHelp(help || p_argc == 1);    // HELP
    displayVersion(version);             // VERSION

    if (role == e_dealer) {
      fileMustExist(task_file, R_OK);
    }
  }
  catch(std::exception & e)
  {
    FATAL(BadOption, "Processing options", e.what());
  }    

  return(true);
}


// ======================================================================
// Private Functions
// ======================================================================


void ProgramOptions::displayHelp(bool const p_condition) throw()
{
  if (! p_condition)
  {
    return;
  }

  string message = "\n";
  stringstream ss("");

  ss << "-M --set-master : specify the node name for the 'master'. If name matches $HOSTNAME, assign the 'master' role [" << master_name << "]" << endl;
  ss << "-W --worker     : assign the 'worker' role" << endl;
  ss << "-C --controller : assign the 'controller' role" << endl;
  ss << "-H --host-list  : comma-separated list of node names on which to spin up controllers" << endl;
  ss << "-L --log-dir    : directory where mcqr will write log files [" << log_dir << "]" << endl;
  ss << "-h --help       : show this help information" << endl;
  ss << "-v --version    : show version information" << endl;
  ss << "-V --verbose    : turn on verbose debugging [=" << verbose << "]" << endl;
  ss << "-t --tasks      : task file (REQUIRED)" << endl;
  ss << "-p --prog-file  : progress file - write progress to the specified text file" << endl;
  ss << "-c --max-cores  : max number of worker cores per node, where 0 = unlimited [=" << max_cores << "]" << endl;
  ss << "-l --max-load   : max load on machine (% - as an integer), where 0 = unlimited. [=" << max_load << "]" << endl;
  ss << "-m --max-memory : max memory usage (% - as an integer), where 0 = unlimited [=" << max_memory << "]" << endl;
  ss << "-w --task-wait  : minimum time (in seconds) between launching any two tasks on a node (allows fractional seconds) [=" << std::fixed << std::setw(4) << std::setprecision(2) << task_wait << "]" << endl;
  ss << "-d --temp-dir   : directory (on node or global FS) where mcqueuer can write temporary files [=" << tmp_dir << "]" << endl;

  message += ss.str();
  message += "\nExamples:\n\n";
  message += "./mcqr -t /path/to/myfile.txt -M " + get_host_name() + " -c 32 -l 80 -m 85 -w 15.2 -V\n\n";
  cerr << message;
  exit(EXIT_SUCCESS);
}


void ProgramOptions::displayVersion(bool const p_condition) throw()
{
  if (! p_condition)
  {
    return;
  }
/*

  string cpr;
  cpr += "   Copyright (C) 2011 Pacific Northwest National Laboratory\n\n";

  string dis;
  dis += "This software was prepared as an account of work sponsored by\n";
  dis += "an agency of the United States Government. Neither the United\n";
  dis += "States Government nor any agency thereof, nor Battelle Memorial\n";
  dis += "Institute, nor any of their employees, makes any warranty,\n";
  dis += "express or implied, or assumes any legal liability or\n";
  dis += "responsibility for the accuracy, completeness, or usefulness\n";
  dis += "of any information, apparatus, product, or process disclosed,\n";
  dis += "or represents that its use would not infringe privately owned\n";
  dis += "rights. Reference herein to any specific commercial product,\n";
  dis += "process, or service by trade name, trademark, manufacturer, or\n";
  dis += "otherwise does not necessarily constitute or imply its\n";
  dis += "endorsement, recommendation, or favoring by the United States\n";
  dis += "Government or any agency thereof, or Battelle Memorial Institute.\n";
  dis += "The views and opinions of authors expressed herein do not\n";
  dis += "necessarily state or reflect those of the United States Government\n";
  dis += "or any agency thereof.\n\n";
  dis += "   PACIFIC NORTHWEST NATIONAL LABORATORY\n";
  dis += "       operated by BATTELLE for the\n";
  dis += "    UNITED STATES DEPARTMENT OF ENERGY\n";
  dis += "     under Contract DE-AC05-76RL01830\n";

//-D'SBLAST_VERSION="1_0_1"'
//-D'CXX_VER="GNU /usr/local/gcc_4.8.2/bin/g++ -v 4.8.2"'
//-D'CXX_OPTIMIZE_LEVEL="-I/usr/local/gcc_4.8.2/include/c++/4.8.2"'
//-D'CXX_DEBUG_LEVEL="OFF"'

#ifndef APP_NAME_LOWER
#define APP_NAME_LOWER "ERROR"
#endif
#ifndef APP_NAME_UPPER
#define APP_NAME_UPPER "ERROR"
#endif
#ifndef APP_DESC
#define APP_DESC "ERROR"
#endif
#ifndef BOOST_VER
#define BOOST_VER "ERROR"
#endif
#ifndef CXX_VER
#define CXX_VER "ERROR"
#endif
#ifndef CXX_OPTIMIZE_LEVEL
#define CXX_OPTIMIZE_LEVEL "ERROR"
#endif
#ifndef CXX_DEBUG_LEVEL
#define CXX_DEBUG_LEVEL "ERROR"
#endif
#ifndef SVN_TAG_VER
#define SVN_TAG_VER "ERROR"
#endif
#ifndef SVN_VER
#define SVN_VER "ERROR"
#endif

  string message;
  message  = "\n"
             "\n   " APP_DESC "\n"
             "\n   " APP_NAME_LOWER " version:           " SBLAST_VERSION " (" SVN_VER ")" 
             "\n   Compiled with:            " CXX_VER
             "\n     Optimize Level:         " CXX_OPTIMIZE_LEVEL
             "\n     Debug:                  " CXX_DEBUG_LEVEL
             "\n     Boost library version:  " BOOST_VER
             "\n   Compiled on:              " __DATE__ ", " __TIME__
             "\n\n";
  message += cpr;
  message += dis;
  writeLog(message);

//#undef BOOST_VER
//#undef CXX_VER
//#undef SVN_TAG_VER
//#undef SVN_VER
*/

cerr << "VERSION NOT IMPLEMENTED" << endl;
  exit(EXIT_SUCCESS);
}


//  Function used to check that 'option' is used
void ProgramOptions::requiredOption(string const & p_option, string p_value) throw(logic_error)
{
  if ("" == p_value)
  {
    throw(logic_error("Missing a required option '" + p_option + "'."));
  }

  return;
}


void ProgramOptions::checkEnum(string const & p_option, int unsigned p_value, int unsigned const p_check0, int unsigned const p_check1)
{
  if ((p_check0 != p_value) && (p_check1 != p_value))
  {
    FATAL(RangeError, "Exception", "The value of " + p_option + ", " + ToString<int unsigned>(p_value) + ", must be " + ToString<int unsigned>(p_check0) + " or " + ToString<int unsigned>(p_check1) + ".");
  }
  return;
}


void ProgramOptions::checkRange(string const &p_option, int unsigned p_value, u_int64_t const p_check0, u_int64_t const p_check1)
{
  if (p_check0 > p_value)
  {
    FATAL(RangeError, "Exception", "The value of " + p_option + ", " + ToString<int unsigned>(p_value) + ", must be greater than or equal to " + ToString<int unsigned>(p_check0) + ".");
  }
  else if (p_check1 < p_value)
  {
    FATAL(RangeError, "Exception", "The value of " + p_option + ", " + ToString<int unsigned>(p_value) + ", should be less than or equal to " + ToString<int unsigned>(p_check1) + ".");
    }

  return;
}


void ProgramOptions::fileMustExist(string const & p_file, int const p_flag) throw(logic_error)
{

  if (0 != access(p_file.c_str(), p_flag))
  {
    throw(logic_error("Required file, '" + p_file + "', does not exist"));
  }

  std::ifstream in(p_file.c_str(), std::ifstream::ate | std::ifstream::binary);
  if (in.tellg() < 1)
  {
    throw(logic_error("Required file, '" + p_file + "', is empty"));
  }

  return;
}
