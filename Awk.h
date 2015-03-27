//enable awk-style text processing
// .gz files are handled automagically by the gzstream class

#ifndef AWK_H
#define AWK_H
#include "util.h"
#include <iostream>
#include <fstream>
#include "gzstream.h"

using namespace std;
class Awk {
  ifstream file;
  igzstream zfile;
  char* record;
  uint len; // length of record
  StringSplitter split;
  bool gz; // true if it's a .gz file
  
public:
  int nr; // no. of records read
  int nf; // no. of fields found in current record
  string line; // equivalent to $0

  Awk(void) : len(0), gz(false), nr(0){}
  ~Awk(void){
    if(len > 0)delete[] record;
  }
  bool open(const char* fname);
  void close(void){
    if(gz)zfile.close();
    else file.close();
  }
  int next(char fs = ' ', char rs = '\n');
  char* field(int i){return i < nf? split[i]: nullptr;} // equivalent to $i
  bool eof(void){
    if(gz)return zfile.eof();
    else return file.eof();
  }
  
};




#endif

