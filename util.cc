#include "util.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cctype>

void deblank(char* p){ // eliminate ' ' and '\t' from string
  char* q = p;

  while(*p = *q++){
    if(!isblank(*p))p++;
  }
} 

int StringSplitter::operator()(char* record, char fs){
  nf = 1;
  field.clear();
  field.push_back(record); // set the first pointer
  while(*record){
    if( (fs == ' ' && *record == '\t') || *record == fs){ // terminate string and store next start position
      *record = '\0';
      while(isblank(*++record)); // skip leading blanks
      field.push_back(record);
      nf++;
    }
    else record++;
  }
  return nf;
}

char* StringSplitter::operator[](int i){
  if(i < 0 || i >= nf) return NULL;
  return field[i];
}


void split_string(char* line, std::vector<char*>& token, char fs){ // input: char* line with
  //fields delimited by fs, and vector<char*> token, an empty array  of c-strings.
  //output: '\0' replaces fs, making line a sequence of c-strings whose addresses are stored 
  //in token.
  token.clear();
  token.push_back(line);
  while(*line){
    if(*line == fs){
      *line++ = '\0';
      token.push_back(line);
    }
    else line++;
  }
}



// quick and dirty printf-type format string adapter for c++ streams
// usage, e.g:  cout << format("Now we can do formatted output such as six = %d, pi = %f, etc",6, 3.14159)<<endl;
// set MAXCHARS appropriately 

std::string format(const char* fstr,...){
  char buf[MAXCHARS+1]; // will truncate after MAXCHARS characters -- no buffer overflows
  va_list ptr;
  //int nchars;

  va_start(ptr,fstr);
  /*nchars = */ vsnprintf(buf,MAXCHARS,fstr,ptr);
  //cout << "format: vsnprintf returns "<< nchars<<endl;
  va_end(ptr);
  std::string output(buf);
  return output;
}

//***************************

std::string str_time(double t){
  int it = (int)t;
  time_t tt = it;
  double ft = t-it;
  const char* fmt = "%.2d:%.2d:%7.4f";

  struct tm* pl = localtime(&tt);
  //std::cout << "localtime sets isdst to "<<pl->tm_isdst<<std::endl;
  return format(fmt,pl->tm_hour,pl->tm_min,pl->tm_sec + ft);
}

std::string str_date(double t){
  time_t tt = (int)t;
  struct tm* pl = localtime(&tt);
  std::ostringstream s;
  
  s <<  pl->tm_mon+1 <<"/"<<pl->tm_mday<<"/"<<pl->tm_year+1900;
  return s.str();
}

double get_time(int date, std::string time){
  std::stringstream ss;
  struct tm pl;
  char buf[18];


  ss.str("");
  ss <<date<<time<<":00";
  strncpy(buf,ss.str().c_str(),18);
  //std::cout << "calling strptime with "<<buf<<std::endl;

  strptime(buf,"%Y%m%d%H:%M:%S",&pl);
  //std::cout <<"tm: "<<pl.tm_sec<<" "<<pl.tm_min<<" "<<pl.tm_hour<<" "<<pl.tm_mday<<" "<<pl.tm_mon<<" "<<pl.tm_year<<" "<<pl.tm_wday<<" "<<pl.tm_yday<<" "<<pl.tm_isdst<<std::endl;
  //std::cout << "get_time has isdst = "<<pl.tm_isdst<<std::endl;
  pl.tm_isdst = -1; // fixing a bug (design defect?) in strptime
  strftime(buf,sizeof(buf), "%s", &pl);
  //std::cout << "mktime returns "<<mktime(&pl)<<" strftime has "<<buf<<std::endl;
  return atof(buf);
}

uint32_t String::hash1(uintptr_t salt) const{ // simple hash function
  int i,j;
  uint32_t sum = 0;
  const char* s = this->c_str();
  int n = strlen(s);
  int m = (n < 4? 4:n);
  for(i = j = 0; i < m;i++){
    if(j == 0) sum ^= salt<<i;
    unsigned char c = (i >= n ? ' ': s[i]); // pad short strings with blanks
    sum += c <<(8*j);
    j = (j+1)%4;
    //      cout << format("s[%d] = %c, sum = %d\n",i,s[i],sum)<<endl;
  }
  //  std::cout << this<<" hashing "<<s<<" to "<<sum<<std::endl;

  return sum;
}

uint32_t String::hash(uintptr_t salt) const{ // simple hash function
  uint32_t b;
  uint8_t* b0 = (uint8_t*)&b;
  int j0,j1,j2,j3;
  /*  b0[0] = salt&0xff;
  b0[1] = (salt>>8)&0xff;
  b0[2] = (salt>>16)&0xff;
  b0[3] = (salt>>24)&0xff;
  */
  b = salt;
  const char* s = this->c_str();

  for(int i = 0;s[i];i++){ // mix in s
    j0 = i&3;
    j1 = (i+1)&3;
    j2 = (i+2)&3;
    j3 = (i+3)&3;
    b0[j0] += (uint8_t)s[i] + b0[j3]*b0[j2] + b0[j1];
  }
  for(int i = 0;i < 8;i++){ // additional mixing
    j0 = i&3;
    j1 = (i+1)&3;
    j2 = (i+2)&3;
    j3 = (i+3)&3;
    b0[j3] += (b0[j0]*b0[j1]) + b0[j2];
  }
  //std::cout << " hashing "<<s<<" to "<<b<<std::endl;
  
  return b;
} 
