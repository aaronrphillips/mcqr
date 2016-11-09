///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved. 
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
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <signal.h>
#include <sstream>
#include <unordered_map>
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"
#include "SysStats.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "global"


// Namespaces
using namespace std;


u_int16_t const g_NORMALEXIT                 =   0;
u_int16_t const g_ABNORMALEXIT               =   1;
double          g_max_cpu_percentage         = 90.0;
double          g_max_mem_percentage         = 90.0;
double          g_max_num_children           =  5.0;
bool            g_verbose                    = false;
string          g_exe_name                   = "";

//  Static inits
sigar_t     * SysStats::my_sigar     = NULL;

int g_interrupted = 0;

// Getters/Setters

void setExeName(string name) throw()
{
  g_exe_name = name;
}

string getExeName() throw()
{
  return g_exe_name;
}

double getMaxCpuPercentage(void) throw()
{
  return(::g_max_cpu_percentage);
}


double getMaxMemPercentage(void) throw()
{
  return(::g_max_mem_percentage);
}


double getMaxNumberChildren(void) throw()
{
  return(::g_max_num_children);
}


void setMaxMemPercentage(double const p_value) throw()
{
  ::g_max_mem_percentage = p_value;
  return;
}


void setMaxNumberChildren(double const p_value) throw()
{
  ::g_max_num_children = p_value;
  return;
}


void fatal (string const & p_ecode, string const & p_item, string const & p_context, string const & p_class, string const & p_func, string const & p_file, int const p_line) throw()
{
  u_int16_t pad = 14;
  cerr << "\nFATAL ERROR:\n";
  cerr << setw(pad) << "File:  "     << p_file        << "(line:" << p_line << ")\n";
  cerr << setw(pad) << "Class:  "    << p_class       << "\n";
  cerr << setw(pad) << "Function:  " << p_func        << "\n";
  cerr << setw(pad) << "Message:  "  << "Error Code=" << p_ecode  << "\n";
  cerr << setw(pad) << " "           << p_item        << "\n";
  cerr << setw(pad) << " "           << p_context     << "\n"     << endl;
  exit(1);
}



bool getVerbose(void) throw()
{
  return g_verbose;
}

void setVerbose(bool verbose) throw()
{
  g_verbose = verbose;
}

int max(int a, int b)
{
  return (a > b) ? a : b;
}

int min(int a, int b)
{
  return (a < b) ? a : b;
}

void checkValues(vector<string> values, int num_expected, string description)
{
  if (values.size() != num_expected)
  {
    cerr << "ERROR: unexpected number of message parts for message: " << description << ". Expected " << num_expected << " but found " << values.size() << endl;
    for (int i = 0; i < values.size(); ++i)
    {
      cerr << i << ":" << values[i] << ":" << endl;
    }
    exit(-1);
  }
}

void g_signal_handler (int signal_value)
{
    g_interrupted = 1;
}

void g_catch_signals ()
{
#if (!defined(WIN32))
    struct sigaction action;
    action.sa_handler = g_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
#endif
}

