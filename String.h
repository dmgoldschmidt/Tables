#ifndef STRING_H
#define STRING_H
#include "Array.h"
#include <string>

/* Derived from Array for heap management.  See Array.h. 
 */
class String : public Array<char>{
 public: 
  String(void) : Array<char>() {}
  String(const char* s) : Array<char>(strlen(s)){
    for(int i = 0;i < N;i++)((char*)(buf+2*sizeof(int)))[i] = s[i];
  }
  String(int n) : Array<char>(n) {}
  String(const string& s) : Array<char>(s.size()){
    for(int i = 0;i < N;i++)((char*)(buf+2*sizeof(int)))[i] = s[i];
  }
  String(const String& s) : Array<char>(s) {}
  String& operator=(const char* s){ // deep copy
    reset(strlen(s));
    for(int i = 0;i < N;i++)((char*)(buf+2*sizeof(int)))[i] = s[i];
  }
  bool operator <=(String& s) const {
    int n = min(N,s.len());
    for(int i = 0;i < n;i++) if( ((char*)(buf+2*sizeof(int)))[i] > s[i]) return false;
    return true;
  }
};

std::ostream& operator <<(std::ostream& os, String& s){
  for(int i = 0;i < s.len();i++)os << s[i];
  return os;
} 

#endif    

    
    
