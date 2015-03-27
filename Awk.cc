#include <string.h>
#include "Awk.h"

bool Awk::open(const char* fname){
  StringSplitter namesplitter;
  char tbuf[strlen(fname)+1];
  strcpy(tbuf,fname);
  nr = 0;
  int ln = namesplitter(tbuf,'.');
  //  cout << "last field: "<<namesplitter[ln-1]<<endl;
  if(strcmp(namesplitter[ln-1],"gz") == 0){
    zfile.open(fname);
    gz = true;
    return zfile.good();
  }
  else{
    file.open(fname);
    gz = false;
    return file.good();
  }
}

int Awk::next(char fs, char rs){ // read next record and split it into fields
  if(gz){
    getline(zfile,line,rs);
    if(zfile.eof()) return 0;
  }
  else{
    getline(file,line,rs);
    if(file.eof())return 0;
  }
  if(line.size() >= len){ // buffer not big enough
    if(len > 0)delete[] record;
    len = line.size() + 1;
    record = new char[len];
  }
  strcpy(record,line.c_str());
  nf = split(record,fs);
  nr++;
  return nf;
}
