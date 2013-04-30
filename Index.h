#ifndef INDEX_H
#define INDEX_H
#include "Array.h"
#include <sstream>
#include <cstdarg>
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
  template<typename T>
  String(const T& x){ // also add type conversions from int, double, char, and other classes 
    std::stringstream iss;
    iss << x;
    *this = iss.str();
  }
};


uint32_t String::hash(uintptr_t salt) const{ // simple hash function
  int i,j;
  uint32_t sum = 0;
  const char* s = this->c_str();

  for(i = j = 0; s[i] != '\0';i++){
    if(j == 0) sum ^= salt<<i;
    sum += s[i]<<(8*j);
    j = (j+1)%4;
    //      cout << format("s[%d] = %c, sum = %d\n",i,s[i],sum)<<endl;
  }
  //std::cout << this<<" hashing "<<s<<" to "<<sum<<std::endl;
  return sum;
}


template <typename KEY>
class Index;

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
    if(val == 99){
      cout << format("buf[%d] = %x\n",i,&buf[i]);
    }
 
    while(buf[i].value >= 0){ // slot occupied (collision)
      if(buf[i].next.len() == 0){ //make new Index if there isn't one already
	buf[i].next = Buf(bufsize); // allocate new buffer
      }
      buf = buf[i].next;
      i = key.hash((uintptr_t)&buf)%bufsize;
      if(val == 99){
	cout << format("buf[%d] = %x\n",i,&buf[i]);
      }
    }
    buf[i].key = key;
    buf[i].value = val;
  }

  void del(const KEY& key){
    KeyItem<KEY>* key_item = find(key);
    if(key_item != (KeyItem<KEY>*)NULL){
      key_item->value = -1; // delete it
    }
  }

  KeyItem<KEY>* find(const KEY& key){
    int i = key.hash((uintptr_t)this)%bufsize;
    Buf buf = items;
      
    if(key == String("99")){
      cout << format("buf[%d] = %x\n",i,&buf[i]);
    }
 
    while(buf[i].value >= 0){ // slot occupied
      if(buf[i].key == key) return &buf[i]; // found it
      if(buf[i].next.len() == 0) break; // not found and there's no continuation
      buf = buf[i].next; // search the next index 
      i = key.hash((uintptr_t)&buf)%bufsize;
      if("key" == String("99")){
	cout << format("buf[%d] = %x\n",i,&buf[i]);
      }
 
    }
    return (KeyItem<KEY>*)NULL; // not there
  }
};
#endif
