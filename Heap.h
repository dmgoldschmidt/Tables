#ifndef HEAP_H
#define HEAP_H
//#include <cstddef>
#include <iostream>
#include <fstream>
#include "Array.h"

template<typename T>
class Heap { 
  /* Manages an n-long heap of arbitrary (type T) items online.  Normal usage is to track online the n smallest 
  items from an arbitrary number of items seen one at a time.  The (implicit) heap structure is that the children
  of item[i] are item[2*i+1] and item[2*i+2] (so the parent is item[(i-1)/2]).  Data is added via the add method.  When the
  Heap is full, the new item is added iff it is smaller than the largest item currently in the heap, which it replaces.
  
  With the alternate constructor, the user provides a pre-allocated array of items.  In this mode, the class operates directly
  on the user-supplied array and not on a copy, so the user has write access to the array.  Caution is required here -- if the
  user does not maintain the heap structure, incorrect results can follow.  Thus, the user should
  either: a) enter data into the array only with the 'push' or add methods, or b) run the 'heap' method after any data is modified.
  A standard use of this mode is to just call the 'sort' method, which will do an in-place nlog(n) sort on the array.
  */
protected:
  int length; // max. no. of entries
  int _nitems; // current no. of entries 
  bool in_place; // user supplies storage
  //  T _weight;
  T* item;

  void copy(const Heap& t){
    if(item != (T*)NULL && !in_place) delete[] item;
    _nitems = t._nitems;
    length = t.length;
    item = new T[length];
    for(int i = 0;i < _nitems;i++) item[i] = t.item[i];
  }
  void push(int i){ // all edges of the tree satisfy >= except between i and its parent.  Fix that edge recursively
    if(i == 0)return;
    int j = (i-1)/2; // parent of i
    if(item[j] <  item[i]){ // make parent >= child
      T t = item[i];
      item[i] = item[j];
      item[j] = t;
      push(j);
    }
  }
public:
  Heap(void){item = (T*)NULL;length = 0;} // default constructor
  Heap(int l, T* t = (T*)NULL){item = (T*)NULL;reset(l,t);} // std mode -- we allocate 
  Heap(Array<T>& A){reset(A.dimension(),&A[0]);} // user allocation mode
  ~Heap(void){if(!in_place && item != (T*)NULL)delete[] item;}
  Heap(const Heap& t): in_place(false){item = (T*)NULL;copy(t);}
  Heap& operator=(const Heap& t){copy(t);return *this;}
  virtual void reset(int l, T* t = (T*)NULL){ // (re)-initialization
    if(l <= 0) throw "Heap: heap size must be positive\n";
    if(in_place = (t != (T*)NULL)) item = t; // we hope the user knows what he's doing
    else if(item == (T*)NULL)item = new T[l]; // delayed initialization after default constructor
    else if(length != l){ // re-initialization with new length
      delete[] item;
      item = new T[l];
    }
    length = l;
    _nitems = 0;
  }

  inline int nitems(void){return _nitems;} 
  inline T& operator[](int i) {return item[i];}
  inline void heap(int n){ // make the first n items a heap 
    _nitems = (n > length? length:n);
    for(int i = (_nitems-1)/2;i >= 0;i--)reheap(i);
  }
  inline void heap(void){heap(length);} // heap the entire array
  inline void sort(int n){heap(n);while(pop());} // heapsort the first n items (useful for in-place mode)
  inline void sort(void){sort(length);} // sort the entire array
  inline virtual void push(void) { // add item[_nitems] to the heap, increment _nitems 
    if(_nitems < length) push(_nitems++);
  }

  void reheap(int i) {// the children of this sub-heap are heaps.  Make it a heap recursively
    int i1 = 2*i+1;
    if(i1 >=_nitems) return; 
    int i2 = i1+1;
    T t;

    if(i2 >= _nitems || item[i2] <  item[i1]) i2 = i1; // choose largeer child
    if(item[i] < item[i2]){ // we need parent >= larger child
      t = item[i];item[i] = item[i2];item[i2] = t; // swap parent with larger child
      reheap(i2); // recursively fix up child heap
    }
  }

  virtual bool pop(void){ // swap the first and last items, reheap the remaining items and decrement _nitems
    if(_nitems > 0){
      T t = item[0];
      item[0] = item[--_nitems];
      item[_nitems] = t;
      //    std::cout << "nitems = "<<_nitems<<", ";
      reheap(0);
      return true;
    }
    return false; // nothing happened
  }
  

  virtual bool add(const T& new_item){ // return true iff new_item was in the top n
    if(_nitems < length){ // add it to the array
      item[_nitems] = new_item; // put it at the end
      push(_nitems++); // move it up
    }
    else{// when the heap is full and the new item is in the bottom n, replace item[0] with the new item
      if(item[0] < new_item) return false;
      //      _weight += new_item.weight-item(0)._weight;
      item[0] = new_item; // old top is clobbered here
      reheap(0);
    }
    return true;
  }

  void dump(int i = 0){
    if(_nitems <= i){
      std::cout << std::endl;
      return;
    }
    std::cout << item[i]<<" ";
    dump(2*i+1);
    dump(2*i+2);
  }
};

template<typename T>
class WeightedHeap : public Heap<T> { // tracks total weight (assumes T has a member double T::weight)
  double _weight;
public:
  WeightedHeap(int l = 1, double w = 0) : Heap<T>(l) {_weight = w;Heap<T>::item[0].weight = 0;}
  inline double weight(void){return _weight;}
  inline void reset(int l){Heap<T>::reset(l);_weight = 0;}
  inline bool pop(void){
    _weight -= Heap<T>::item[0].weight;
    return Heap<T>::pop();
  }
  inline void push(void){_weight += Heap<T>::item[Heap<T>::_nitems].weight;Heap<T>::push();}
  inline bool add(const T& new_item){
    double w = Heap<T>::_nitems < Heap<T>::length? 0 : Heap<T>::item[0].weight; // get the right weight increment
    if(Heap<T>::add(new_item)){
      _weight += new_item.weight - w;
      return true;
    }
    return false;
  }
};




// template<typename T>
// class WeightedHeap : Heap{
//   double _weight;
//   //  void copy(const WeightedHeap& t){Heap<T>::copy(t);weight = t.weight;}
// public:
//   // Heap(T* t, int l, int n = 0) : Heap<T>::(t,l,n){_weight = 0;} //in-place constructor (dangerous!)
//   Heap(int l = 1) : Heap<T>(l){_weight = 0;} // standard constructor (safe)
//   WeightedHeap(const WeightedHeap& t) : Heap<T>(t){_weight = t._weight;}
//   WeightedHeap& operator=(const WeightedHeap& t){Heap<T>::copy(t);_weight = t._weight;return *this;}
//   void reset(int l){Heap<T>::reset(l);_weight = 0;}
//   inline double weight(void){return _weight;}
  

#endif

//   void remove(int i){ // recursively remove node i
//     int i1,i2; 
//     if((i1 = 2*i+1) >= _nitems) return; // the hole is at a leaf node
//     i2 = i1+1;
//     if(i2 >= _nitems || item(i1) < item(i2))i2 = i1;
//     item(i) = item(i2);
//     remove(i2);
//   }
    
// // the following adds functionality to track the total weight of all items in the heap (item->weight must be defined)
// // this is done by redefining the type of _nitems to Wint

// template <typename T> // next four lines are nonsense needed to avoid compiler warning
// class Wint;

// template< typename T> 
// std::ostream& operator << (std::ostream& os, const Wint<T>& i);

// template< typename T>
// struct Wint {
//   friend std::ostream& operator << <>(std::ostream& os, const Wint<T>& i);
//   int n;
//   double w;
//   T* item;

//   //  Wint(void){n = 0;w = 0;item = (T*)NULL;}
//   explicit Wint(int nn){n = nn;w = 0;item = (T*)NULL;}
//   operator int() {return n;}
//   Wint& operator=(int nn){n = nn;}
//   // inline bool operator < (int i)const {return n < i;}
//   //inline int operator++ (void){w += item[n++].weight;return n;} 
//   inline int operator-- (void){w -= item[--n].weight;return n;} // prefix (note: last item is item[_nitems-1])
//   inline int operator++ (int){w += item[n].weight;return n++;} // postfix
//   //  inline int operator-- (int){int nn = n;w += item[n--].weight;return nn;}
//   //inline int operator-(int i){return n-i;}
// };

// template <typename T> 
// std::ostream& operator << (std::ostream& os, const Wint<T>& i){
//   os << "("<<i.n<<","<<i.w<<") ";
// }


