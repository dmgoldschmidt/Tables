#ifndef UTIL_H
#define UTIL_H
#include<cstdarg>
#include<string>
#include<sstream>
#include<sys/time.h>
#include<time.h>
#define MAXCHARS 1000
#include <cmath>
#include <vector>
//#include <stdint.h>
using namespace std;

class StringSplitter { // Functor to split a C-string into fields using field separator fs.  NOTE: leading
  // blanks in a field are skipped, so the field separator  is really fs followed by any number of blanks.
  // If fs == ' ', the splitter will also recognize '\t'.  If fs == '\t', ONLY '\t' is recognized.

  char fs; // field separator
  int nf; // no. of fields split out
  vector<char*> field; // array of ptrs to fields
public:
 StringSplitter(char fs0 = ' '): fs(fs0), nf(0), field(10) {} // allocate 10 fields just to get started
  int operator()(char* record); // input a record, return nf (Note: fs is over-written with '\0', making
  // record a sequence of C-strings)
  char* operator[](int i);
};


void split_string(char* line, std::vector<char*>& token, char fs = ' ');
void deblank(char* string);
std::string format(const char* fstr,...);
std::string str_time(double t);
std::string str_date(double t);
double get_time(int date, std::string time);
class InternalTime { // floating pt seconds <-> 38-bit integer microseconds (3.18 days = 2^38 microseconds)
  int a;
  char b;
  inline void convert(double delta){
    double n = delta*1000000/128;
    a = (int) n; // high-order signed 31 bits
    b = (char) ((n-a)*128); // low-order signed 7 bits
  }
public:
  InternalTime(double delta = 0){ 
    if(fabs(delta) > 259200) throw format("InternalTime: bad input %lf\n",delta); // abs(delta) >= 3 days
    convert(delta);
  }
  inline InternalTime& operator=(double delta){convert(delta); return *this;}
  inline InternalTime& operator=(const InternalTime& t){a = t.a;b = t.b; return *this;}
  inline operator double() const{return (a*128.0 + b)/1000000.0;}
};
class String : public std::string { // example of a useable KEY class (see KeyIndex.h) 
public:
  String(void) : std::string() {}
  String(const std::string& s) : std::string(s){}
  String(const char* p) : std::string(p){}
  uint32_t hash(uintptr_t salt = 0) const; // hash function
  uint32_t hash1(uintptr_t salt = 0) const; // hash function
  template<typename T>
  String(const T& x){ // also add type conversions from int, double, char, and other classes 
    std::stringstream iss;
    iss << x;
    *this = iss.str();
  }
};

#endif
