///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2014.  PNNL.  All Rights Reserved.
//
// CODING CONVENTIONS:
//    * Class names are CamelCase with first word upper case
//    * Functions are camelCase with first word lower case
//    * Function parameters are lower case with _ and have a_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <cassert>
#include <iostream>
// External Includes
#include <sigar.h>
#include <sigar_format.h>
// Internal Includes
// Application Includes
#include "SysStats.hpp"
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "SysStats"


// Namespaces used

using namespace std;


//==============================================================================
// Class SysStats
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


SysStats::SysStats(
                  string const & p_host,
                  double         p_max_load,  // 0 - 100
                  double         p_max_memory // 0 - 100
                ) throw() :
my_host(p_host),
max_load(p_max_load),
max_memory(p_max_memory)
{
  if (NULL == this->my_sigar)
  {
    int result = sigar_open(&this->my_sigar);
    assert(SIGAR_OK == result);
  }

if (getVerbose()) { cout << "SYSSTATS:" << my_host << ":" << ":" << max_load << ":" << fixed << max_memory<< ":" << endl; }
  assert(0.0 <= max_load);
  assert(0.0 <= max_memory);

  return;
}


// ======================================================================
// Methods
// ======================================================================


bool SysStats::SysCanDoMoreWork(void) throw()
{
  return(LoadLessThanMax() && MemLessThanMax());
}


bool SysStats::LoadLessThanMax(void) throw()
{
  sigar_cpu_info_list_t  list;
  int                    result = sigar_cpu_info_list_get(this->my_sigar, &list);
  assert(SIGAR_OK == result);

  sigar_cpu_info_t      &system           = list.data[0];
  //string                 vendor           = system.vendor;
  //string                 model            = system.model;
  //int                    mhz              = system.mhz;
  //u_int32_t              cache_size       = system.cache_size;
  //double                    total_sockets    = system.total_sockets;
  double                    total_cores      = system.total_cores;
  //int                    cores_per_socket = system.cores_per_socket;

if (getVerbose())
{
//cout << "VENDOR     :" << system.vendor           << ":" << endl;
//cout << "MODEL      :" << system.model            << ":" << endl;
//cout << "MHZ        :" << system.mhz              << ":" << endl;
//cout << "CACHE SIZE :" << system.cache_size       << ":" << endl;
cout << "TOTAL SOCKE:" << system.total_sockets    << ":" << endl;
cout << "TOTAL CORES:" << system.total_cores      << ":" << endl;
cout << "MAX LOAD:" << (total_cores  * (max_load / 100.0)) << ":" << endl;
//cout << "CORES/SOCK :" << system.cores_per_socket << ":" << endl;
}

  //  LOAD
  sigar_loadavg_t        load;
  result = sigar_loadavg_get(this->my_sigar, &load);
  assert(SIGAR_OK == result);

  // load.loadavg[0]  =  1 minute load
  // load.loadavg[1]  =  5 minute load
  // load.loadavg[2]  = 15 minute load

if (getVerbose())
{
cout << "LOAD1      :" << load.loadavg[0]  << ":" << endl;
cout << "LOAD5      :" << load.loadavg[1]  << ":" << endl;
//cout << "LOAD15     :" << load.loadavg[2]  << ":" << endl;
}

  sigar_cpu_info_list_destroy(this->my_sigar, &list);
  //return((load.loadavg[0] < system.total_cores) && (load.loadavg[1] < system.total_cores));
  return((load.loadavg[0] < (total_cores  * (max_load / 100.0))) && (load.loadavg[1] < (total_cores * (max_load / 100.0))));
}


bool SysStats::MemLessThanMax(void) throw()
{
  //  MEMORY
  sigar_mem_t mem;
  int         result = sigar_mem_get(this->my_sigar, &mem);
  assert(SIGAR_OK == result);

  double ram         = mem.ram         * getKiB();
  double total       = mem.total       / getKiB();
  double used        = mem.used        / getKiB();
  double free        = mem.free        / getKiB();
  double actual_used = mem.actual_used / getKiB();
  double actual_free = mem.actual_free / getKiB();
  //double mem_max     = mem.total       / getKiB() * 0.9;

if (getVerbose())
{
cout << "RAM        :" << ram         << ":" << endl;
cout << "TOTAL      :" << total       << ":" << endl;
cout << "USED       :" << used        << ":" << endl;
cout << "FREE       :" << free        << ":" << endl;
cout << "ACTUAL_USED:" << actual_used << ":" << endl;
cout << "ACTUAL_FREE:" << actual_free << ":" << endl;
}

  return((mem.actual_used / getKiB()) < (this->max_memory * (total / 100)));
}


bool SysStats::CanFitBytes(u_int64_t bytes) throw()
{
  sigar_mem_t mem;
  int         result = sigar_mem_get(this->my_sigar, &mem);
  assert(SIGAR_OK == result);

  return bytes < mem.free;
}

//string SysStats::print(void) const throw()
//{
//  return("NOT IMPLEMENTED");
//}


// -------------------------------------
// Friends
// -------------------------------------


//ostream & operator<<(ostream & p_os, SysStats const * p_sysstat) throw()
//{
//  return(p_os << p_sysstat->print());
//}


//ostream & operator<<(ostream & p_os, SysStats const & p_sysstat) throw()
//{
//  return(p_os << p_sysstat.print());
//}
