#ifndef INDEX_H
#define INDEX_H
#include "/home/david/code/include/Array.h"
#include <iostream>
#include <sstream>
#include <cstdarg>
#include <cstring>

#define MAXCHARS 1000

template <typename KEY, typename VALUE = unsigned int>
struct KeyValue {
  VALUE value; 
  KEY key;
  bool occupied;  
  KeyValue(void) : occupied(false) {}
  KeyValue(const KEY& k, const VALUE& v) : value(v), key(k), occupied(true) {} 
};

template<typename KEY, typename VALUE>
std::ostream& operator<<(std::ostream& os, const KeyValue<KEY,VALUE>& kv){
  if(kv.occupied) std::cout << "key: " << kv.key << " value: " << kv.value;
  else std::cout<< "unoccupied ";
  return os;
}

template<typename KEY, typename VALUE>class IndexIterator;

template<typename KEY, typename VALUE = unsigned int>
class Index { // dynamically expandable hash table, based on Array
  friend class IndexIterator<KEY,VALUE>;
  typedef KeyValue<KEY,VALUE> KV;
  Array<KV> items;
  uint blocksize; 
  uint nsearch;

  uint get_slot(const KEY& key){ // find an empty slot for key
    uint hashmask = 123456789;

    for(int nbuf = 0;;nbuf++){
      int i = key.hash(nbuf*hashmask)%blocksize;
      for (uint j = 0; j < nsearch; j++){ 
	uint k = nbuf*blocksize + (i+j)%blocksize; // wrap-around search
	if(!items[k].occupied || items[k].key == key){ // found empty slot or key
	  return k;
	}
      }
    }// nothing available in this block.  On to the next
  }


public:
  Index(uint b = 100, uint n = 10) : items(b), blocksize(b), nsearch(n) {}
  // methods

  KV* find(const KEY& key){
    uint slot = get_slot(key);
    return (items[slot].occupied? &items[slot] : nullptr); 
  }

  bool has(const KEY& key){
    return items[get_slot(key)].occupied;
  }

  // void del(const KEY& key){
  //   KV* p = find(key);
  //   if(p != (KV*)NULL){
  //     p->occupied = false;
  //   }
  // }
  
  VALUE& operator[](const KEY& key){
    uint slot = get_slot(key);
    if(!items[slot].occupied){ // store new key
      items[slot].key = key;
      items[slot].occupied = true;
    }
    return items[slot].value;
  }

  IndexIterator<KEY,VALUE> end(void) const {
    IndexIterator<KEY,VALUE> it(this);
    it.current_item = items.len();
    return it;
  }

  IndexIterator<KEY,VALUE> begin(void) const {
    IndexIterator<KEY,VALUE> it(this);

    if(!items[0].occupied) it.step();
    return it;
  }
  
  void dump(void) const {
    int n = items.len();
    for(int i = 0;i < n;i++){
      cout <<i<<": "<< items[i]<<endl;
    }
  }
}; // end class Index



template<typename KEY, typename VALUE = unsigned int>
class IndexIterator : public std::iterator<std::forward_iterator_tag, KeyValue<KEY,VALUE> > {
  friend class Index<KEY,VALUE>;
  const Index<KEY,VALUE>* index;
  int current_item;

  void step(void){ // step to next occupied slot
    if(++current_item >= index->items.len()) return;
    while(current_item < index->items.len()){ 
      if(index->items[current_item].occupied){
	//	cout << format("\nindex @%x: stepped to item %d\n",index,current_item);
	return;
      }
      current_item++;
    }

    //    std::cout << format("step: found occupied slot %d at %x: ",current_item, &current_index->items[current_item])<< current_index->items[current_item]<<std::endl; 
  }

public:
  IndexIterator(void) : index(nullptr), current_item(0) {}
  IndexIterator(const Index<KEY,VALUE>* ind) : index(ind), current_item(0) {}
  IndexIterator(const IndexIterator& it) : index(it.index), current_item(it.current_item) {}
    
  KeyValue<KEY,VALUE>* operator->(void) const{
    return &(index->items[current_item]);
  }
    
  KeyValue<KEY,VALUE>& operator*(void) const{
    return index->items[current_item];
  }

  IndexIterator& operator++(void) { // ++it
    step();
    return *this;
  }

  IndexIterator operator++(int){ // it++
    IndexIterator invalue(*this); // return initial state 
    step();
    return invalue;
  }
  
  bool operator==(const IndexIterator& it){
    if(index != it.index) return false;
    if(current_item != it.current_item) return false;
    return true;
  }

  bool operator!=(const IndexIterator& it){return !operator==(it);}
};
#endif


  // const KV& start(void){
  //   current_buf = this;
  //   current_item = 0;
  //   if(items[0].occupied) return items[0]; 
  //   return step();
  // }

  // const KV& step(void){ // step to next unoccupied slot
  //    do{
  //      if(end())break;
  //      current_item++;
  //      if(current_item >= items.len()){
  // 	 current_item = 0;
  // 	 current_buf = current_buf->next;
  //      }
  //   }while(!current_buf->items[current_item].occupied);
  //   return current_buf->items[current_item];
  // }

  // bool end(void){
  //   return (current_item == items.len()-1 && current_buf->next == (Index*)NULL);
  // }
//   void dump(void){
//     int nitems = 0;
//     for(int i = 0;i < items.len();i++){
//       if(items[i].occupied) nitems++;
//     }
//     std::cout << format("occupancy for %x: %f\n",this,nitems*1.0/items.len());
//     if(next != (Index*)NULL)next->dump();
//   } 
// };
