///////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2008.  PNNL.  All Rights Reserved.
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


#ifndef OSUTIL_HPP
#define OSUTIL_HPP


// System Includes
#include <string>
#ifndef _MSC_VER
#include <syslog.h>
#include <sys/types.h>
#else
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOGDI
#include <windows.h>
#include <process.h>
#endif
// External Includes
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "osUtil"

// Typedefs
#ifndef _MSC_VER
	typedef int FileDescriptor;
	#define FileDescriptorNull -1;
#else
	typedef HANDLE FileDescriptor;
	#define FileDescriptorNull NULL;

	typedef          __int64   int64_t;
	typedef unsigned __int64 u_int64_t;
	typedef          __int32   int32_t;
	typedef unsigned __int32 u_int32_t;
	typedef          __int16   int16_t;
	typedef unsigned __int16 u_int16_t;
	typedef           __int8    int8_t;
	typedef unsigned  __int8  u_int8_t;
#endif


// Windows Defines
#ifdef _MSC_VER

//#define __restrict__ __restrict

#define fmax(a,b) (((a)>=(b))?(a):(b))
#define fmin(a,b) (((a)<=(b))?(a):(b))
#define rint(a)   (((a)>=0.f)?floor((a)+0.5f):ceil((a)-0.5f))

#endif


// Namespaces
using namespace std;


// Getters/Setters

string const & getPathSeparator(void) throw();
bool   const   isLinux(void) throw();
string const   expandPath(string const & p_path) throw();
string const   getEnvVar(const string & p_var) throw();
bool   const   weCanDoMore(const string & p_value) throw();
string const   get_host_name(void) throw();
string const   trim_host_name(string long_name) throw();
string const getExePath() throw();

#endif // OSUTIL_HPP
