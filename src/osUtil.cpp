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


#include "config.h"
// System Includes
#include <iostream>
#include <limits.h>
#ifdef HAVE_STRING_H
  #include <string>
#endif
// #ifndef _MSC_VER // groups are linux-specific
  #ifdef HAVE_ERRNO_H
    #include <errno.h>
  #endif
  #ifdef HAVE_GRP_H
    #include <grp.h>
  #endif
  #include <arpa/inet.h>
  //#include <linux/types.h>
  //#include <linux/if_packet.h>
  #include <net/ethernet.h>
  #include <net/if.h>
  #if HAVE_STDLIB_H
    #include <stdlib.h>
  #endif
  #include <sys/ioctl.h>
  #include <sys/socket.h>
  #include <sys/types.h>
  #ifdef HAVE_UNISTD_H
    #include <unistd.h>
  #endif
// #endif
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "osUtil.hpp"
//#include "sysinfo.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "osUtil"


// Namespaces
using namespace std;


// Getters/Setters

string const & getPathSeparator(void) throw()
{
#ifndef _MSC_VER
  static string path_separator("/");
#else
  static string path_separator("\\");
#endif
  return(path_separator);
}


bool const isPriviledgedUser(void) throw()
{
#ifndef _MSC_VER
  return((0 == getuid()) || (0 == geteuid()));
#else
  // TODO: Handle 2009/01/06 MS stuff
  return(true);
#endif

}

inline bool file_exists(const std::string& name) 
{
  return ( access( name.c_str(), F_OK ) != -1 );
}

// from http://stackoverflow.com/a/933996
string const getExePath() throw()
{
  char buf[PATH_MAX];
  // try to get the path from /proc/self/exe
  int num_bytes = readlink("/proc/self/exe", buf, PATH_MAX);
  string path(buf, num_bytes);
  if (! path.empty())  
  {
    return path;
  }
  if (getExeName().at(0) == '/')
  {
    return path; // if the exe was run as an absolute path, just return that
  }
  if (getExeName().find_first_of('/') != string::npos)
  {
    return getEnvVar("PWD") + "/" + getExeName(); // if relative, just concatenate to PWD
  }

  // search through the users PATH
  string search_path(getEnvVar( "PATH" ));
  istringstream ss(search_path);

  while (!ss.eof())
  {
    string dir;
    getline(ss, dir, ':');
    path = dir + "/" + getExeName();

    if (file_exists(path))
    {
      return path;
    }
  }

  return getExeName();  // should we just return NULL or "" instead?
}

bool const isLinux(void) throw()
{
#ifdef __linux__
  return(true);
#else
  return(false);
#endif
}

string const getEnvVar(const string & p_var) throw()
{
  return string(getenv(p_var.c_str()));
}


string const get_host_name(void) throw()
{
  static char buf[256];
  gethostname(buf,sizeof buf);
  string s(buf);
  return s.substr(0, s.find_first_of("."));
}

string const trim_host_name(string long_name) throw()
{
  return long_name.substr(0, long_name.find_first_of("."));
}

bool const weCanDoMore(const string & p_value) throw()
{
  
  bool cpu_ok   = 1;
  bool load1_ok = 1;
  bool load5_ok = 1;
  bool mem_ok   = 1;

/*

// sigar_t singleton to take advantage of the cache
sigar_t *_sigar = NULL;

sigar_t *sigar() {
  sigar_open(&_sigar);

  return _sigar;
}


  double avg1;
  double avg5;
  double avg15;

  double a1;
  double a2;
  double a3;
  double percent_idle_cpu;
  double a5;
  double a6;
  double a7;
  double a8;

  a1 = a2 = a3 = percent_idle_cpu = a5 = a6 = a7 = a8 = 0.0;

  eight_cpu_numbers(&a1, &a2, &a3, &percent_idle_cpu, &a5, &a6, &a7, &a8);
  loadavg(&avg1, &avg5, &avg15);
  meminfo();

  bool cpu_ok   = (percent_idle_cpu < getMaxCpuPercentage());
  bool load1_ok = (avg1 < getMaxNumberChildren());
  bool load5_ok = (avg5 < getMaxNumberChildren());
  bool mem_ok   = (kb_main_used / kb_main_total < getMaxMemPercentage());

  sigar_loadavg_t m;
  sigar_loadavg_get(sigar(), &m);

  double loadavg[3];
  loadavg[0] = m.loadavg[0];
  loadavg[1] = m.loadavg[1];
  loadavg[2] = m.loadavg[2];

cerr << "WCDM:" << p_value << ":" << percent_idle_cpu << ":" << loadavg[0] << ":" << loadavg[1] << ":" << avg1 << ":" << avg5 << ":" << kb_main_total << ":" << kb_main_used << ":" << endl;
*/

  return(cpu_ok && load1_ok && load5_ok && mem_ok);
}


/*
sub we_can_do_more()
{
  my $meminfo  = new SysMemInfo();
  my $sysinfo  = new SysInfo();

  my $load1_ok = ($sysinfo->getload_1() < ($SLURM_ENV->get_num_cores() * $MAX_LOAD_PERCENTAGE)) ? 1 : 0;
  my $load5_ok = ($sysinfo->getload_5() < ($SLURM_ENV->get_num_cores() * $MAX_LOAD_PERCENTAGE)) ? 1 : 0;
  my $mem_ok   = ($meminfo->get_percent_used_memory() < $MAX_MEM_PERCENTAGE)                    ? 1 : 0;  
  my $core_ok  = (cpu_core_we_can_use() > -1)                                                   ? 1 : 0;  
  my $result   = ($load1_ok && $load5_ok && $mem_ok && $core_ok);

  if ($DEBUG && $result)
  {
    writelog("INFO: We can do more work.\n");
  }

  if ($DEBUG && ! $result)
  {
    writelog("INFO: load1_ok:$load1_ok; load5_ok:$load5_ok; mem_ok:$mem_ok; core_ok:$core_ok; result:$result; meminfo:" . $meminfo->get_percent_free_memory() . ":" . $MAX_MEM_PERCENTAGE . ".\n");
  }

  return($result);
}
*/
