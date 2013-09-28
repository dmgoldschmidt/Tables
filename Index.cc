#include "Index.h"
#include "util.h"
#include <cstdint>
#include <cassert>
#include <iostream>

using namespace std;


int main(int argc, char** argv){
  Index<String> I(100);
  typedef KeyValue<String> KeyVal;
  for(int i = 0;i < 100;i++){
    cout << "i = "<<i<<endl;
    if(i == 78){
      cout << "we're here\n";
    }
    I[String(i)] = i;
  }
  cout << "*******\n";
  
  for(int i = 0;i < 100;i++){
    KeyValue<String,uint>* p = I.find(String(i));
    if(p == nullptr) cout << "Didn't find key "<<String(i)<<endl;
    else {
      assert( i == p->value);
      assert(I[String(i)] == i);
    }
  }
  cout << "I has not there:  "<<I.has("not_there")<<endl;
  cout<< "I has 100: "<< I.has(String(100))<<endl;

  I[String(100)] = 4141;
  assert(4141 == I[String(100)]);

}
