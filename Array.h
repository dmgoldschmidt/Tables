#ifndef ARRAY_H
#define ARRAY_H
#include <iostream>
#include <cassert>
//#include <vector>
#include "util.h"
//#define DEBUG

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif



template <typename ITEM>
class Array; 

template <typename ITEM>
struct Initializer { // subclass this for a callback initialization functor
  virtual void operator()(ITEM& item) = 0; // if specified, it is called once for each allocated ITEM
};

template <typename ITEM>
struct SimpleInit : public Initializer<ITEM> { //like this -- just set up a pointer to some constant
  const ITEM* fill;
  SimpleInit(const ITEM* p) : fill(p) {} // save the constant fill
  void operator()(ITEM& item){item = *fill;}
};

template <typename ITEM>
class ArrayBlock{ 
  friend class Array<ITEM>; // private helper class for Array
  size_t n; // block size
  ITEM *data; // block of n items
  ArrayBlock* next; // pointer to next block
  uint refcount; // only used in the first block
  Initializer<ITEM>* init; // user defined initialization functor
  const ITEM* fill; // alternative initialization to constant

  // ArrayBlock(size_t nn, const ITEM* f) : n(nn), next(nullptr), refcount(0), fill(f) {

  //   data = new ITEM[n];
  //   DBG(std::cout << format("new ArrayBlock at %x, data at %x\n",this,data);)
  //   if(fill != nullptr)
  //     for(uint i = 0;i < n;i++)data[i] = *fill;
  // }
  ArrayBlock(size_t nn, Initializer<ITEM>* in) : n(nn), next(nullptr), refcount(0), init(in), fill(nullptr) {

    data = new ITEM[n];
    DBG(std::cout << format("new ArrayBlock at %x, data at %x\n",this,data);)
    if(init != nullptr)
      for(uint i = 0;i < n;i++)(*init)(data[i]); // call user-defined functor to initialize each ITEM 
  }

  ArrayBlock(size_t nn, const ITEM* item) : n(nn), next(nullptr), refcount(0), init(nullptr), fill(item) {

    data = new ITEM[n];
    DBG(std::cout << format("new ArrayBlock at %x, data at %x\n",this,data);)
    if(fill != nullptr)
      for(uint i = 0;i < n;i++)data[i] = *fill; // initialize with constant
  }
  ArrayBlock(const ArrayBlock&); // no copy
  ArrayBlock& operator=(const ArrayBlock&); //no assignment

  ~ArrayBlock(void){
    DBG(std::cout << format("~ArrayBlock: deleting %x and %x\n",this,data);)
    delete[] data;
    if(next != nullptr)delete next;
  }

  uint len(void){ // recursive 
    return n + (next == nullptr? 0: next->len());
  }

  size_t blocksize(void){return n;}

  ITEM& operator[](uint i){ // recursive
    DBG(std::cout << format("ArrayBlock %x: operator [%d]\n",this,i);)
    if(i >= n){ // on to the next block
      DBG(std::cout << format("index %d exceeds block length\n",i);)
	//	exit(1);
	//      fflush(stdout);
      if(next == nullptr){
        if(init != nullptr) next = new ArrayBlock(n,init);
        else next = new ArrayBlock(n,fill);
      }
      return next->operator[](i-n);
      
    }
    return data[i];
  }
};

template<typename ITEM> 
class Array {
  //  typedef void(*Initializer)(ITEM&);
  //  friend ostream& operator <<(ostream& os, const Array& A);
protected:
  ArrayBlock<ITEM>* first; // first (and possibly only) block of data and control info
public:
  Array(void) : first(nullptr){
    DBG(std::cout << format("Array %x void constructor\n",this);)
  }
  // Array(size_t n, const ITEM* fill = nullptr) : first(new ArrayBlock<ITEM>(n,fill)){
  //   first->refcount = 1;
  //   DBG(std::cout << format("Array %x: first block at %x\n",this,first);)
  // }
  Array(size_t n, Initializer<ITEM>* init) : first(new ArrayBlock<ITEM>(n,init)){
    first->refcount = 1;
    DBG(std::cout << format("Array %x: first block at %x\n",this,first);)
  }
  Array(size_t n, const ITEM* fill = nullptr) : first(new ArrayBlock<ITEM>(n,fill)){
    first->refcount = 1;
    DBG(std::cout << format("Array %x: first block at %x\n",this,first);)
  }
  Array(initializer_list<ITEM> l){
    int n = l.size();
    first = new ArrayBlock<ITEM>(n,(ITEM*)NULL);
    first->refcount = 1;
    const ITEM* p = l.begin();
    for(int i = 0;i < l.size();i++)(*first)[i] = p[i];
  }

  Array(const Array& A): first(A.first) {
    if(first != nullptr) first->refcount++; // shallow copy
    DBG(if(first != nullptr) std::cout << format("Array %x (Block %x) copy constr: refcount = %d\n",this,first,first->refcount);) 
   }
  ~Array(void){
    DBG(if(first != nullptr) std::cout<< format("~Array %x (Block %x): refcount = %d\n",this,first,first->refcount);) 
    if( first != nullptr && first->refcount-- <= 1)delete first;
  }
  void reset(size_t n, Initializer<ITEM>* init){
    DBG(if(first != nullptr) std::cout << format("Array %x(Block %x) reset: refcount = %d\n",this,first,first->refcount);) 
    if(first != nullptr && first->refcount-- <= 1)delete first;
    first = new ArrayBlock<ITEM>(n,init);
    first->refcount = 1;
  }

  void reset(size_t n, const ITEM* fill = nullptr){
    DBG(if(first != nullptr) std::cout << format("Array %x(Block %x) reset: refcount = %d\n",this,first,first->refcount);) 
    if(first != nullptr && first->refcount-- <= 1)delete first;
    first = new ArrayBlock<ITEM>(n,fill);
    first->refcount = 1;
  }

  Array& operator=(const Array& A){ // shallow copy -- switch pointer to new first block
    DBG(if(first != nullptr) std::cout << format("Array %x(Block %x) assignment: refcount = %d\n",this,first,first->refcount);)
    if(first != nullptr && first->refcount-- <= 1)delete first;
    first = A.first;
    if(first != nullptr)first-> refcount++;
    DBG(if(first != nullptr) std::cout << format("Array %x(Block %x) assignment: refcount = %d\n",this,first,first->refcount);)
    return *this;
  }
  Array copy(void) const{ // deep copy returning object 
    Array<ITEM> A;
    if(first != nullptr){
      A.reset(first->n,first->init);
      int n = len(); // only call recursive len once
      for(int i = 0;i < n;i++) A[i] = this->operator[](i);
    }
    return A;
  }
  int len(void) const {
    if(first == nullptr) return 0;
    return first->len();
  }
  ITEM& operator[](int i)const {
    if(first == nullptr)throw "Array bounds error\n";
    return first->operator[](i);
  }
  bool operator==(Array& A) const{
    int n = len();
    if(n != A.len())return false;
    for(int i = 0;i < n;i++) if(this->operator[](i) != A[i])return false;
    return true;
  }
  bool operator!=(Array& A) const{return !operator==(A);}
};

template<typename ITEM>
std::ostream& operator <<(std::ostream& os, Array<ITEM>& A){
  int n = A.len();
  for(int i = 0;i < n;i++)os << A[i]<<" ";
  return os;
} 

#endif
/*#define REFCOUNT (((int*)buf)[0])
#define N (((int*)buf)[1])
#define DATA ((ITEM*)(buf+Hlen*sizeof(int)/sizeof(ITEM) + 1)) // Note: is this right?
*/
  
  // void reset(const ITEM* p){first->reset(p);}
  // void reset(const ITEM& fill){first->reset(fill);}
  // void reset(void){first->reset();}
  //   if(first != nullptr){
  //     if(n == N) return; //NOP
  //     if(--REFCOUNT <= 0)delete[] buf;
  //   }
  //   int nitems = n + Hlen*sizeof(int)/sizeof(ITEM) + 1;
  //   buf = new ITEM[nitems];
  //   DBG << format("allocated Array of %d items of length %d at %x\n",nitems,sizeof(ITEM),buf); 
  //   REFCOUNT = 1;
  //   N = n;
  // }

// template <typename ITEM, size_t N>
// class ArrayBlock{ // linked list of data blocks and control info
//   friend class Array<ITEM,N>;
//   ITEM data[N];
//   //  uint start;
//   ArrayBlock* next;
//   uint refcount;
//   ITEM fill;
//   bool filling;

//   ArrayBlock(void) : next(nullptr), refcount(0), filling(false) {}
//   ArrayBlock(const ITEM& f) : fill(f), next(nullptr), refcount(0), filling(true) {
//     for(uint i = 0;i < N;i++)data[i] = fill;
//   }
//   ArrayBlock(const ArrayBlock&); // no copy
//   ArrayBlock& operator=(const ArrayBlock&); //no assignment

//   ~ArrayBlock(void){
//     if(next != nullptr)delete next;
//   }

//   uint len(void){
//     return N + (next == nullptr? 0: next->len());
//   }

//   ITEM& operator[](uint i){
//     DBG(format("ArrayBlock %x: operator [%d]\n",this,i))
//     if(i >= N){ // on to next block
//       DBG(format("index %d exceeds block length\n",i))
//       fflush(stdout);
//       if(next == nullptr){
// 	next = filling? new ArrayBlock(fill): new ArrayBlock();
// 	DBG(format("new ArrayBlock at %x\n",next))
// 	fflush(stdout);
//       }
//       return next->operator[](i-N);
      
//     }
//     return data[i];
//   }
// };
