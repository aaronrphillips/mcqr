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


#ifndef QUACK_HPP
#define QUACK_HPP


// System Includes
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
// External Includes
#include <zmq.hpp>
// Internal Includes
// Application Includes
#include "global.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "MessageWrapper"


// Namespaces used
using namespace std;


class MessageWrapper
{

public:

  // Getters & Setters

  inline wrapper_type_enum getMessageWrapperType(void)
  {
    return(message_type);
  }


  inline zmq::message_t & getMessage(void)
  {
    return(message);
  }


  inline vector<string> getValues(void)
  {
    return(values);
  }


  // Constructors

  MessageWrapper(
        wrapper_type_enum  const & p_type,
        vector<string>   const & p_values,
        u_int8_t         const   p_size_digits = 8
       ) throw();

  MessageWrapper(zmq::message_t &     p_message,
        u_int8_t       const p_size_digits = 8
       ) throw();

  // Destructor

  ~MessageWrapper(void) throw()
  {
    return;
  }


  // Public Methods

  string print(void) const throw();


private:

  // Getters/Setters

  // Constructors

  MessageWrapper(void) throw();
  MessageWrapper(MessageWrapper const & p_sequence) throw();


  // Operators

  // Methods


  // Variables

  wrapper_type_enum message_type;
  zmq::message_t  message;
  vector<string>  values;
  u_int8_t        size_digits;

friend
  ostream & operator<<(ostream & p_os, MessageWrapper const * p_data) throw();
friend
  ostream & operator<<(ostream & p_os, MessageWrapper const & p_data) throw();

};


#endif  // QUACK_HPP
