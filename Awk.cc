#include <string.h>
#include "Awk.h"

bool Awk::open(const char* fname){
  file.open(fname);
  nr = 0;
  return file.good();
}

int Awk::next(void){ // read next record and split it into fields
  getline(file,line,rs);
  if(file.eof())return 0;
  if(line.size() >= len){ // buffer not big enough
    if(len > 0)delete[] record;
    len = line.size() + 1;
    record = new char[len];
  }
  strcpy(record,line.c_str());
  nf = split(record);
  nr++;
  return nf;
}
