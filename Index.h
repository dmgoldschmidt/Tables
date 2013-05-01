#ifndef INDEX_H
#define INDEX_H
#include "Array.h"
#include <sstream>
#include <cstdarg>
#include <cstring>
#define MAXCHARS 1000


std::string format(const char* fstr,...){
  char buf[MAXCHARS+1]; // will truncate after MAXCHARS characters -- no buffer overflows
  va_list ptr;
  int nchars;

  va_start(ptr,fstr);
  nchars = vsnprintf(buf,MAXCHARS,fstr,ptr);
  //cout << "format: vsnprintf returns "<< nchars<<endl;
  va_end(ptr);
  std::string output(buf);
  return output;
}

class String : public std::string { // example of a useable KEY class (see KeyIndex.h) 
public:
  String(void) : std::string() {}
  String(const std::string& s) : std::string(s){}
  String(char* p) : std::string(p){}
  uint32_t hash(uintptr_t salt = 0) const; // hash function
  uint32_t hash1(uintptr_t salt = 0) const; // hash function
  template<typename T>
  String(const T& x){ // also add type conversions from int, double, char, and other classes 
    std::stringstream iss;
    iss << x;
    *this = iss.str();
  }
};


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
  std::cout << this<<" hashing "<<s<<" to "<<sum<<std::endl;

  return sum;
}

uint32_t String::hash(uintptr_t salt) const{ // simple hash function
  uint32_t b;
  uint8_t* b0 = (uint8_t*)&b;
  int j0,j1,j2,j3;
  b0[0] = salt&0xff;
  b0[1] = (salt>>8)&0xff;
  b0[2] = (salt>>16)&0xff;
  b0[3] = (salt>>24)&0xff;
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
  //  std::cout << " hashing "<<s<<" to "<<b<<std::endl;
  
  return b;
} 


//template <typename KEY>
//class Index;

template <typename KEY>
struct KeyItem {
  typedef Array<KeyItem<KEY> > Buf;
  Buf next; // initially an empty Array (no allocation) 
  int value; // -1 for unoccupied
  KEY key;
  KeyItem(void) : value(-1) {}
  KeyItem(const KEY& k, int v) : value(v), key(k) {} 
};

template<typename KEY>
class Index {
  typedef Array<KeyItem<KEY> > Buf;
  int bufsize;
  Buf items;

public:
  Index(unsigned b) : bufsize(b){
    items.reset(bufsize); // allocate Array (and auto-initialize KeyItems)
  }  
  // methods

  void add(const KEY& key, int val){
    int i = key.hash((uintptr_t)this)%bufsize;
    Buf buf = items;
    //    if(val == 99){
    //      cout << format(" &buf[0] = %x, i = %d\n",&buf[0], i);
    //    }
 
    while(1){ 
      for (int j = 0; j < bufsize/10; j++){
	int k = (i+j)%bufsize; // wrap-around search
	if(buf[k].value < 0){ // found an empty slot
	  buf[k].key = key;
	  buf[k].value = val;
	  return;
	}
      }
	  
      // nothing available in this buffer.  On to the next
      if(buf[i].next.len() == 0){ //make new buffer if there isn't one already
	buf[i].next.reset(bufsize); // allocate new buffer
	cout << format("collision at i = %d, buffer %x\n",i,&buf[0]);
      }
      buf = buf[i].next;
      i = key.hash((uintptr_t)&buf[0])%bufsize;
      //      if(val == 99){
      //	cout << format("&buf[0] = %x, i = %d\n",&buf[0], i);
      //      }
    }
  }

  void del(const KEY& key){
    KeyItem<KEY>* key_item = find(key);
    if(key_item != (KeyItem<KEY>*)NULL){
      key_item->value = -1; // delete it
    }
  }

  int find(const KEY& key){
    int i = key.hash((uintptr_t)this)%bufsize;
    Buf buf = items;
      
    //    if(key == String("99")){
    //      cout << format("buf[%d] = %x\n",i,&buf[i]);
    //    }
 
    while(buf[i].value >= 0){ // slot occupied
      for (int j = 0; j < bufsize/10;j++){
	int k = (i+j)%bufsize; // wrap-around search
	if(buf[k].key == key) return buf[k].value; // found it
      }
      if(buf[i].next.len() == 0) break; // not found and there's no continuation
      buf = buf[i].next; // search the next index 
      i = key.hash((uintptr_t)&buf[0])%bufsize;
      //      if(key == String("99")){
      //	cout << format("&buf[0] = %x, i = %d\n",&buf[0], i);
      //      }
 
    }
    return -1; // not there
  }
  void dump(void){dump(items);}
  void dump(Buf b){ // recursively called
    int nitems = 0;
    for(int i = 0;i < bufsize;i++){
      if(b[i].value >= 0){
	nitems++;
	if(b[i].next.len() != 0) dump(b[i].next);
      }
    }
    cout << format("occupancy for %x: %f\n",&b[0],nitems*1.0/bufsize);
  } 
};
#endif
