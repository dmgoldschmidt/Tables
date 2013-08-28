#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include "util.h"
#include "Index.h"
#include "Array.h"
#include "Matrix.h"
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
  Table add_col(String& name, int type);
  int nrows(void){return _nrows;}
  
};

Table::Table(int nr,Array<String>names,Array<int>types){
  _nrows = nr;
  nstr = ndbl = 0;

  for(int i = 0;i < names.len();i++){
    if(types[i] == 0){
      columns[names[i]] = Pair<int>(nstr++,types[i]);
      //      cout << format("Table: column %d: %s(string)\n",i,names[i].c_str());
    }
    else{ 
      columns[names[i]] = Pair<int>(ndbl++,types[i]);
      //      cout << format("Table: column %d: %s(double)\n",i,names[i].c_str());
    }
  }
  Doubles.reset(_nrows);
  Strings.reset(_nrows);
  for(int i = 0;i < _nrows;i++){
    Doubles[i].reset(ndbl);
    Strings[i].reset(nstr);
  }
}

TableRow& TableRow::operator++(void) { // ++it 
  if(++row > table->_nrows)row = table->_nrows;
  return *this;
}

TableRow TableRow::operator++(int){ // it++
  TableRow invalue(*this); // return initial state 
  if(++row > table->_nrows)row = table->_nrows;
  return invalue;
}
TableEntry TableRow::operator[](String& col){return (*table)(row,col);}


TableEntry Table::operator()(int i, String& col){
  TableEntry tmp_ref;
  Pair<int> p = columns[col]; // should check that col exists
  if(p.y == 0)
    tmp_ref(Strings[i][p.x]);
  else
    tmp_ref(Doubles[i][p.x]);
  return tmp_ref;
}

// Table Table::add_col(String& name, int type){
//   Table new_table;
//   new_table.columns = columns.copy();
//   new_table._nrows = _nrows;

//   if(type == 0){// add a String col
    
    

  


#endif
