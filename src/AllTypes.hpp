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


#ifndef ALLTYPES_HPP
#define ALLTYPES_HPP

// System Includes
#include <map>
#include <memory>
#include <string>
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "osUtil.hpp"
#include "global.hpp"
//#include "Exception.hpp"


// Namespaces

using namespace std;


typedef enum
{
  e_diagonal,
  e_insert,
  e_delete,
  e_yikes
} traceback_t;


typedef struct AffineCell
{
  int32_t diagonal;
  int32_t left;
  int32_t up; 
  int32_t pad;
  AffineCell(void) throw() : diagonal(-9999), left(-9999), up(-9999) {}
} AffineCell_t;


typedef union ArrowCell
{
  u_int8_t    allbits;
  struct
  {
    u_int8_t  diagonal_diagonal:1;      //  M_M       bit 0
    u_int8_t  diagonal_left:1;          //  M_Iy      bit 1
    u_int8_t  diagonal_up:1;            //  M_Ix      bit 2
    u_int8_t  left_diagonal:1;          //  Iy_M      bit 3
    u_int8_t  left_left:1;              //  Iy_Iy     bit 4
    u_int8_t  up_diagonal:1;            //  Ix_M      bit 5
    u_int8_t  up_up:1;                  //  Ix_Ix     bit 6
    u_int8_t  pad:1;                    //  unused    bit 7
  };
} ArrowCell_t;


typedef std::shared_ptr< map<string, std::vector<string> > > sharedNmerMap;

#endif // ALLTYPES_HPP
