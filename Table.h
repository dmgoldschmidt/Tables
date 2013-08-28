#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include "util.h"
#include "Index.h"
#include "Array.h"
// #include "Matrix.h"
//extern template class Matrix<String>;
//extern template class Matrix<double>;
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
  TableEntry& operator=(const String& ss){
    *s = ss;
    return *this;
  }
  TableEntry& operator=(const double dd){
    *d = dd;
    return *this;
  }
  
  operator double&(void) {
    if(d != nullptr) return *d;
    throw "TableEntry object type error\n";
  }

  operator String&(void) {
    if(s != nullptr) return *s;
    throw "TableEntry object type error\n";
  }

  const char* c_str(void){
    if(s != nullptr)return s->c_str();
    if(d != nullptr)return String(*d).c_str();
    throw "TableEntry object type error\n";
  }
};

ostream& operator<<(ostream& os, const TableEntry& x);

template<typename T>
struct Pair{
  T x;
  T y;
  Pair(void) {}
  Pair(T xx, T yy) : x(xx), y(yy) {}
};

template<typename T>
ostream& operator<<(ostream& os, const Pair<T>& p){
  os << "("<<p.x<<","<<p.y<<")";
  return os;
}

class Table;
class TableView;


class TableRow : public std::iterator<std::forward_iterator_tag, TableRow>{
  Table* table;
  int row;
public:
  //  TableRow(void) : table(nullptr), row(0) {}
  TableRow(Table* t, int r = 0) : table(t), row(r)  {}
  TableRow* operator->(void){return this;}
  TableRow& operator*(void){return *this;}
  TableRow& operator++(void);
  TableRow operator++(int);
  
  bool operator==(const TableRow& r){
    if(table != r.table) return false;
    if(row != r.row) return false;
    return true;
  }
  bool operator!=(const TableRow& it){return !operator==(it);}
  TableEntry operator[](String& col);
};

Table csv_read(char* fname, int nrecs = 100);
class Table {
  friend class TableRow;
  int _nrows; // no. of rows
  int ndbl; // no. of double columns
  int nstr; // no. of String columns
  Array<Array<double>> Doubles;
  Array<Array<String>> Strings;
  Index<String,Pair<int> > columns;
public:
  Table(int nr,Array<String>names,Array<int>types);
  TableEntry operator()(int i, String& col);
  TableRow operator[](int i){return TableRow(this,i);}
  TableRow begin(void){return TableRow(this);}
  TableRow end(void){return TableRow(this,_nrows);}
  void add_col(String& name, int type);
  int nrows(void){return _nrows;}
  
};



#endif
