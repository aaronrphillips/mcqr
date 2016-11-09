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


#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP


// System Includes
#include <map>
#include <string>
// External Includes
// Internal Includes
// Application Includes


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "Exception"


// Namespaces
using namespace std;


//
//  DEVELOPER NOTE:
//    Renamed Unknown to UnknownException due to a name collision
//    in the Winioctl.h _MEDIA_TYPE enum
//
static const string AlphaNumeric("AlphaNumeric");
static const string BadOption("BadOption");
static const string FakeData("FakeData");
static const string FileIO("FileIO");
static const string FloatingPoint("FloatingPoint");
static const string IllegalValue("IllegalValue");
static const string NotFound("NotFound");
static const string PermissionDenied("PermissionDenied");
static const string RangeError("RangeError");
static const string SegFault("SegFault");
static const string Shutdown("ShutDown");
static const string Signal("Signal");
static const string TSNH("TSNH");
static const string Unsupported("Unsupported");
static const string UnknownException("Unknown Exception");


class Exception
{

public:

  // Getters/Setters

  static map<string, string> * getErrorCodes(void) throw();


private:

  // Constructors

  Exception(void);
  Exception(Exception const & p_exception);

  // Operators

  Exception & operator=(Exception const & p_exception);


  // Destructor
  
  ~Exception(void) throw()
  {
    return;
  }

};

#endif
