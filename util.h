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
#include <stdint.h>
using namespace std;

class LCG64 { // 64-bit LCG. Parameters from Wikipedia
  const uint64_t a;
  const uint64_t b;
  uint64_t x;
  double uint64_max;
public:
  LCG64(uint64_t seed = 0) : a(2862933555777941757), b(3037000493), uint64_max((double)UINT64_MAX){
    reseed(seed);
  }
  uint64_t step(void){return x = a*x+b;}
  double uniform(void){ return step()/uint64_max;}
  uint64_t reseed(uint64_t seed){
    x = seed;
    for(int i = 0;i < 10;i++)step(); // avoid crappy seeds
    return step();
  }
};

class BloomFilter { // Have we seen a 64-bit key or a string yet? Remember it for next time
  uint64_t word[4];
  LCG64 lcg;
 public:
  BloomFilter(void){}
  bool hash(uint64_t key, bool set = true){
    bool ret = true;
    uint64_t hash = lcg.reseed(key);
    for(int i = 0;i < 8;i++){
      int j = hash & 3; // low order 2-bits
      hash >>= 2;
      uint64_t mask  = 1<<(hash & (0x3f)); // next six bits
      if(mask&word[j] == 0) ret = false; // not yet set
      if(set) word[j] |= mask; // set it
    }
    return ret;
  }
  bool hash(string& s, bool set = true){
    uint64_t hval = 0;
    for(int i = 0;i < s.len();i++){
      hval ^= (s[i] << (i%8));
      return hash(hval);
    }
  }
  bool query(uint64_t key){return hash(key,false);}
  bool query(string& s){return hash(s,false);}
};
      
      



class StringSplitter { // Functor to split a C-string into fields using field separator fs.  NOTE: leading
  // blanks in a field are skipped, so the field separator  is really fs followed by any number of blanks.
  // If fs == ' ', the splitter will also recognize '\t'.  If fs == '\t', ONLY '\t' is recognized.
  int nf; // no. of fields split out
  vector<char*> field; // array of ptrs to fields
public:
  StringSplitter(void):  nf(0), field(10) {} // allocate 10 fields just to get started
  int operator()(char* record,char fs = ' '); /* input a record, split into fields using field separator fs, 
                                               * return nf (Note: fs is over-written with '\0', making
                                               * record a sequence of C-strings)*/
  char* operator[](int i); // subsequent access to the fields

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
  operator int() const{ return hash();}
  template<typename T>
  String(const T& x){ // also add type conversions from int, double, char, and other classes 
    std::stringstream iss;
    iss << x;
    *this = iss.str();
  }
};

#endif
