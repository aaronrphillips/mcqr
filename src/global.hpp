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


#ifndef GLOBAL_HPP
#define GLOBAL_HPP


// System Includes
#include <algorithm> 
#include <cctype>
#include <chrono>
#include <ctime>
#include <functional> 
#include <iomanip>
#include <locale>
#include <string>
#include <sstream>
#include <vector>
// External Includes
// Internal Includes
// Application Includes
#include "osUtil.hpp"


// DEFINITIONS

#define APP_NAME_LOWER "duckblast"
#define APP_NAME_UPPER "DUCKBLAST"
#define APP_DESC "Duck BLAST (" APP_NAME_LOWER ")"
#define SVN_TAG_VER    "0_0_0"

#define FATAL(p_ecode, p_item, p_context) fatal(p_ecode, p_item, p_context, __CLASS__, __func__, __FILE__, __LINE__)


// Namespaces
using namespace std;


extern u_int16_t const g_NORMALEXIT;
extern u_int16_t const g_ABNORMALEXIT;
extern double    const g_nat_log_2;

extern int g_interrupted;

typedef enum
{
  e_dealer     = 1,
  e_controller = 2,
  e_worker     = 3,
  e_all        = 4
} e_discovery_role;

struct null_deleter
{
  void operator()(void const *) const
  {   
  }   
};


template <typename T>
string ToString(const T value)
{
  ostringstream ostr;
  ostr << value;
  return(ostr.str());
}


template <typename T>
T ToNumber(const string & p_value)
{
  T Number;
  stringstream ss(p_value);
  ss >> Number;
  return(Number);
}


typedef enum
{
  e_req_task       =  1,
  e_rep_task       =  2,
  e_rep_wait       =  3,
  e_rep_die        =  4,
  e_processed_task =  5,
  e_none           =  7
} wrapper_type_enum;


void    fatal (string const & p_ecode, string const & p_item, string const & p_context, string const & p_class, string const & p_func, string const & p_file, int const p_line) throw();
bool    getVerbose(void) throw();
void    setVerbose(bool verbose) throw();
void    setExeName(string name) throw();
string  getExeName() throw();


int max(int a, int b);
int min(int a, int b);
void checkValues(vector<string> values, int num_expected, string description);

extern void g_signal_handler (int signal_value);
extern void g_catch_signals ();

// trim from start
static inline std::string ltrim(std::string s) 
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

// trim from end
static inline std::string rtrim(std::string s) 
{
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string trim(std::string s) 
{
  return ltrim(rtrim(s));
}

inline bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

inline string collapseWhitespace(string str)
{
  std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
  str.erase(new_end, str.end()); 
  return str;
}


static inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) 
  {
    elems.push_back(trim(item));
  }
  return elems;
}


static inline std::vector<std::string> split(const std::string &s, char delim) 
{
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}


#endif // GLOBAL_HPP
