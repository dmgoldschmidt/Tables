#ifndef GETOPT_H
#define GETOPT_H
#include<iostream>
#include<vector>
#include<string>

using namespace std;
/* Command line parser
   Constructor creates two n-vectors of strings.  Assumes there are n (option, argument)
   pairs, with the options prefaced by '-'.  If an option or an argument is
   missing, the corresponding string is "".  So arguments without any corresponding
   option can appear anywhere, as can options without an argument.  The isolated 
   string "-" is parsed as an argument.
   
   There is no need to tell the parser what the expected options are, to make them be single
   characters, or to place them all before any arguments without options.
   This syntax is mostly compatible with the usual syntax, with two exceptions:

   1. constructs like "-f filename" are not equivalent to "-ffilename".  The latter
   will produce an option "ffilename" with no argument.
   
   2. If the last option has no argument, but there are additional arguments, the first of
   these will occur as the argument to the last option.  But this is easy to disambiguate
   if the caller knows whether the last option given did or did not take an argument.  
   Alternatively, the caller can define a fake option, like "-/" or whatever one likes,
   to signal the end of the options.

   The convenience method "get" will search for a particular option and return the number of matches
   it finds.  If it finds exactly one and there is a second argument, it sets the second argument to the
   corresponding quantity on the command line. Get will match any prefix of its first argument, hence a long
   first argument can be abbreviated by any of its (unique) prefixes on the command line.


   If the option "-help" appears anywhere on the command line, the help_msg, which defaults to "No help available", 
   is printed and exit(0) is called.

*/

class GetOpt{
  int j;
public:
  vector<string> option;
  vector<string> argument;

  GetOpt(int argc, char** argv, char* help_msg = "No help available\n");
  int get(char* opt); // return nmatches = no. of matches for opt
  int get(char* opt, int& arg);   // return nmatches  and set integer arg iff nmatches == 1
  int get(char* opt, double& arg);// ditto for double 
  int get(char* opt, float& arg);// ditto for float 
  int get(char* opt, string& arg);// ditto for string
};
#endif
