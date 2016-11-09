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
//    * Function parameters are lower case with _ and have a_ prefix
//    * Member variables always use 'this' pointer
///////////////////////////////////////////////////////////////////////////////


// System Includes
#include <cstdio>
// External Includes
// Internal Includes
// Application Includes
#include "MessageWrapper.hpp"


#ifdef  __CLASS__
#undef  __CLASS__
#endif
#define __CLASS__ "MessageWrapper"


//==============================================================================
// Class MessageWrapper
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


MessageWrapper::MessageWrapper(
        wrapper_type_enum  const & p_type,
        vector<string>   const & p_values,
        u_int8_t         const   p_size_digits
       ) throw() :
message_type(p_type),
values(p_values),
size_digits(p_size_digits)
{
  char cmessage_type = (char) message_type;
  stringstream ss;

  ss << cmessage_type;

  for (auto value : values)
  {
    string tmp = value;
    if (value.empty())
    {
      tmp = " "; // need something if we're putting an empty string in. Is there a better character?
    }
    ss << setw(size_digits) << setfill('0') << tmp.size();
    ss << tmp;
  }

//cerr << "MESSAGE WRITE:" << ss.str() << ":" << endl;
  zmq::message_t * temp_message = new zmq::message_t(ss.str().size() + 1);
  memcpy(temp_message->data(), ss.str().c_str(), ss.str().size() + 1);
  message.copy(temp_message);
  delete temp_message;

//cerr << "MESSAGE WRITE:" << string(static_cast<char*>(message.data())) << ":" << endl;
  return;
}


MessageWrapper::MessageWrapper(zmq::message_t & p_message, u_int8_t const p_size_digits) throw() :
size_digits(p_size_digits)
{
  string temp_message(static_cast<char*>(p_message.data())); 

  zmq::message_t * new_message = new zmq::message_t(temp_message.size());
  memcpy(new_message->data(), temp_message.c_str(), temp_message.size());
  message.copy(new_message);
  delete new_message;

  message_type        = (wrapper_type_enum)temp_message.at(0);
  temp_message.erase(0, 1);

  while (! temp_message.empty())
  {
    u_int32_t len = ToNumber<u_int32_t>(temp_message.substr(0, size_digits));
    if (len > temp_message.size()) { cerr << "ERROR: read past end of message. Read len:" << len << " message len:" << temp_message.size() << endl; exit(-1); }
    if (len < 1)                   { cerr << "ERROR: reading zero-length message" << endl; exit(-1); }
    temp_message.erase(0, size_digits);
    values.push_back(temp_message.substr(0, len));
    temp_message.erase(0, len);
  }
  
  return;
}


// ======================================================================
// Methods
// ======================================================================


string MessageWrapper::print(void) const throw()
{
  return("NOT IMPLEMENTED YET\n");
}


// -------------------------------------
// Friends
// -------------------------------------


ostream & operator<<(ostream & p_os, MessageWrapper const * p_message_wrapper) throw()
{
  return(p_os << p_message_wrapper->print());
}


ostream & operator<<(ostream & p_os, MessageWrapper const & p_message_wrapper) throw()
{
  return(p_os << p_message_wrapper.print());
}
