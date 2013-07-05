//enable awk-style text processing

#ifndef AWK_H
#define AWK_H
#include "util.h"
#include <iostream>
#include <fstream>

using namespace std;
class Awk {
  ifstream file;
  char* record;
  int len; // length of record
  StringSplitter split;
  
public:
  char rs; // record separator, defaults to \n
  int nr; // no. of records read
  int nf; // no. of fields found in current record
  string line; // equivalent to $0

 Awk(char fs = ' ', char rs0 = '\n') : len(0),split(fs), rs(rs0), nr(0){}
  ~Awk(void){
    if(len > 0)delete[] record;
  }
  bool open(const char* fname);
  void close(void){file.close();}
  int next(void);
  char* field(int i){return split[i];} // equivalent to $i
};




#endif

