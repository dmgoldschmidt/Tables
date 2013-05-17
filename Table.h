#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include "Index.h"
#include "Array.h"
#include "Matrix.h"

using namespace std;

// struct Col {
//   virtual int item(int i) = 0;
// };
// template <typename T>
// struct Column : public Col {
//   int nitems;
//   Array<T> p;
//   T& item(int i){return p[i];}
//   Column(int n) {
//     p.reset(nitems = n);
//   }
// };


struct TableEntry {
  double* d;
  String* s;
  
  TableEntry(void) : d(nullptr), s(nullptr) {}
  TableEntry(double& x) : d(&x), s(nullptr) {}
  TableEntry(String& x) : d(nullptr), s(&x) {}
  void operator()(double& x){d = &x; s = nullptr;}
  void operator()(String& x){d = nullptr;s = &x;}
  
  operator double&(void) {
    if(d != nullptr) return *d;
    throw "Object type error\n";
  }

  operator String&(void) {
    if(s != nullptr) return *s;
    throw "Object type error\n";
  }

  // Obj& operator=(String& x){
  //   s = &x;
  //   d = nullptr;
  //   return *this;
  // }

  // Obj& operator=(double& x){
  //   d = &x;
  //   s = nullptr;
  //   return *this;
  // }
  
};

ostream& operator<<(ostream& os, const TableEntry& x){
  if(x.d != nullptr) os << *x.d;
  else if (x.s != nullptr) os << *x.s;
  else throw "Object type error\n";
  return os;
}

template<typename T>
struct Pair{
  T x;
  T y;
  Pair(void) {}
  Pair(T xx, T yy) : x(xx), y(yy) {}
};

class Table {
  int nrows; // no. of rows
  int ndbl; // no. of double columns
  int nstr; // no. of String columns
  Matrix<double> Doubles;
  Matrix<String> Strings;
  Index<String,Pair<int> > columns;
  TableEntry tmp_ref;
public:
  Table(int nr,Array<String>names,Array<int>types){
    nrows = nr;
    nstr = ndbl = 0;

    for(int i = 0;i < names.len();i++){
       if(types[i] == 0)
	 columns[names[i]] = Pair<int>(nstr++,types[i]);
       else 
	 columns[names[i]] = Pair<int>(ndbl++,types[i]);
    }
    Doubles.reset(nrows,ndbl,0);
    Strings.reset(nrows,nstr,"");
  }
  TableEntry& operator()(int i, String& col){
    Pair<int> p = columns[col];
    if(p.y == 0)
      tmp_ref(Strings(i,p.x));
    else
      tmp_ref(Doubles(i,p.x));
    return tmp_ref;
  }
};





#endif
