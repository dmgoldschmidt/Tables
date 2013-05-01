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

template <typename KEY, typename VALUE = unsigned int>
struct KeyValue {
  bool occupied;  
  VALUE value; // -1 for unoccupied
  KEY key;
  KeyValue(void) : occupied(false) {}
  KeyValue(const KEY& k, const VALUE& v) : value(v), key(k), occupied(true) {} 
};

template<typename KEY, typename VALUE = unsigned int>
class Index {
  typedef KeyValue<KEY,VALUE> KV;
  Array<KV> items;
  Index* next;
  Index* current_buf;
  int current_item;

  Index(const Index& i); // no copy constructor
  Index& operator=(const Index& i); // no assignment

  // get_slot is private because it can produce duplicate keys
  KV& get_slot(const KEY& key){ // find an empty slot for key
    int bufsize = items.len();
    int i = key.hash((uintptr_t)&items[0])%bufsize;
    for (int j = 0; j < bufsize/10; j++){
      int k = (i+j)%bufsize; // wrap-around search
      if(!items[k].occupied){ // found an empty slot
	items[k].key = key;
	items[k].occupied = true;
	return items[k];
      }
    }
      // nothing available in this buffer.  On to the next
    if(next == (Index*)NULL){ //make new buffer if needed
	next = new Index(bufsize); // allocate new buffer
    }
    return next->get_slot(key); // recurse
  }


public:
  Index(unsigned b = 100) : items(b), next((Index*)NULL), current_buf(this), current_item(0) {}
  ~Index(void){ // recursively delete all the buffers
    if(next != (Index*)NULL) delete next;
  }
  // methods

  KV* find(const KEY& key){
    int bufsize = items.len();
    int i = key.hash((uintptr_t)&items[0])%bufsize;
    for (int j = 0; j < bufsize/10;j++){
      int k = (i+j)%bufsize; // wrap-around search
      if(items[k].occupied && items[k].key == key) return &items[k]; // found it
    }
  
    if(next == (Index*)NULL) return (KV*)NULL; 
    return next->find(key);
  }

  bool has(const KEY& key){
    return (find(key) != (KV*)NULL?  true:false);
  }

  void del(const KEY& key){
    KV* p = find(key);
    if(p != (KV*)NULL){
      p->occupied = false;
    }
  }
  
  VALUE& operator[](const KEY& key){
    KV* p = find(key);
    if( p != (KV*)NULL) return p->value;
    return get_slot(key).value;
  }

  const KV& start(void){
    current_buf = this;
    current_item = 0;
    if(items[0].occupied) return items[0]; 
    return step();
  }

  const KV& step(void){ // step to next unoccupied slot
     do{
       if(end())break;
       current_item++;
       if(current_item >= items.len()){
	 current_item = 0;
	 current_buf = current_buf->next;
       }
    }while(!current_buf->items[current_item].occupied);
    return current_buf->items[current_item];
  }

  bool end(void){
    return (current_item == items.len()-1 && current_buf->next == (Index*)NULL);
  }
  //*****************

  void dump(void){
    int nitems = 0;
    for(int i = 0;i < items.len();i++){
      if(items[i].occupied) nitems++;
    }
    cout << format("occupancy for %x: %f\n",this,nitems*1.0/items.len());
    if(next != (Index*)NULL)next->dump();
  } 
};
#endif
