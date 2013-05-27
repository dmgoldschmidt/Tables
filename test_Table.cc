#include <iostream>
#include "Table.h"
using namespace std;

int main(int argc, char** argv){
  // Array<Col*> A(2);
  // Column<String> s(1);
  // Column<double> d(1);
  // A[0] = &s;
  // A[1] = &d;
  // A[0]->item(0) = String("one");
  // A[1]->item(0) = 1.0;
  // double dd = A[1]->item(0);
  // String ss = A[0]->item(0);
  // cout << ss<< " "<< dd <<endl;

  Array<String> names(2);
  Array<int> types(2);
  names[0] = "name";
  names[1] = "age";
  types[0] = 0;
  types[1] = 1;
  Table t(3,names,types);
  //  cout << format("&names[0]: %x, &names[1]: %x\n",&names[0],&names[1]);
  cout << "nrows: "<<t.nrows<<" ndbls: "<<t.ndbl << " nstr: "<< t.nstr<<endl;
  cout << "Doubles:\n"<<t.Doubles<<"Strings:\n"<<t.Strings<<endl;
  fflush(stdout);
  String name("name"), age("age");
  t(0,name) = "david";
  t(0,age) = 71;
  t(1,name) = "cherie";
  t(1,age) = 55;
  t(2,name) = "jessica";
  t(2,age) = 25;
  for(int i = 0;i < 3;i++){
    cout << t(i,name)<<": "<<t(i,age)<<endl;
 
    String myname;
    myname = t(i,name);
    double myage, myage1;
    myage1 = myage = t(i,age);
    cout <<"myname is "<<myname<<", myage is "<<myage<<" and my other age is "<<myage1<<endl;
  }
  cout << "our combined age is "<< t(0,age)+t(1,age)+t(2,age)<<endl;
  typedef KeyValue<String, Pair<int> > KV;
  for(KV kv = t.columns.start(); !t.columns.end(); kv = t.columns.step()){
    cout << "key: "<<kv.key<<", value: "<<kv.value<<endl;
  }

}
