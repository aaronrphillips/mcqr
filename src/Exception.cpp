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
// External Includes
// Internal Includes
// Application Includes
#include "global.hpp"
#include "Exception.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Exception"


// Namespaces


map<string, string> * Exception::getErrorCodes(void) throw()
{
  static map<string, string> * error_codes = new map<string, string>();
  if (error_codes->empty())
  {
    error_codes->insert(make_pair(AlphaNumeric, "String must be alpha numeric (a-zA-Z0-9_)"));
    error_codes->insert(make_pair(BadOption, "Problem with command line option"));
    error_codes->insert(make_pair(FakeData, "Fake Data"));
    error_codes->insert(make_pair(FileIO, "File IO error"));
    error_codes->insert(make_pair(FloatingPoint, "Floating point error"));
    error_codes->insert(make_pair(IllegalValue, "Illegal value"));
    error_codes->insert(make_pair(NotFound, "Not found"));
    error_codes->insert(make_pair(PermissionDenied, "Permission denied"));
    error_codes->insert(make_pair(RangeError, "Out of range error"));
    error_codes->insert(make_pair(SegFault, "Segmentation fault"));
    error_codes->insert(make_pair(Shutdown, "Shutting down " APP_NAME_LOWER " due to"));
    error_codes->insert(make_pair(Signal, "Signal encountered"));
    error_codes->insert(make_pair(TSNH, "This should not happen"));
    error_codes->insert(make_pair(Unsupported, "Unsupported"));
    error_codes->insert(make_pair(UnknownException, "Unknown error code"));
  }
  return(error_codes);
}
