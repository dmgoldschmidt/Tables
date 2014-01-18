#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include "util.h"
#include "Index.h"
#include "Array.h"
#include <limits>

using namespace std;
static String null(" *NULL* ");
static double NaN = numeric_limits<double>::quiet_NaN();
typedef enum{missing,dble,Str} TableEntryType;
struct TableEntry {
  TableEntryType _type;
  double d;
  String s;

  TableEntry(void) : _type(missing) {}
  TableEntry(double& x) : _type(dble), d(x) {} //set to double
  TableEntry(String& x) : _type(Str), s(x) {} //set to String
  void operator()(double& x){d = x; _type = dble;} //set existing to double
  void operator()(String& x){_type = Str; s = x;} //set existing to String
  TableEntry& operator=(const String& ss){ //set existing to String
    s = ss;
    _type = Str;
    return *this;
  }
  TableEntry& operator=(const double dd){ //set existing to double
    d = dd;
    _type = dble;
    return *this; // test comment
  }
  
  operator double&(void) { //output to double
    if(_type == dble) return d;
    return NaN;
  }

  operator String&(void) { //output to String
    if(_type == Str) return s;
    return null;
  }
  bool operator<(const TableEntry& te) const;
  bool operator==(const TableEntry& te) const;

  const char* c_str(void) const {
    if(_type == Str)return s.c_str();
    if(_type == dble)return String(d).c_str();
    return null.c_str();
  }
  uint32_t hash(uintptr_t salt = 0) const{ // hash function
    if(_type == Str)return s.hash(salt);
    if(_type == dble)return (uint32_t)(d) + salt; // not a great hash function!
    return 0;
  }
  TableEntryType type(void){return _type;}
};

ostream& operator<<(ostream& os, const TableEntry& x);

class Table;

class TableRow {
  friend ostream& operator<<(ostream& os, TableRow& r);
  Table* t;
  Array<TableEntry> r;
public:
  TableRow(void) : t(nullptr) {}
  TableRow(Table* tt, int blocksize = 100) : t(tt), r(100){}
  void reset(Table* tt, int bs = 100){t = tt; r.reset(bs);}
  TableEntry& operator[](int i){return r[i];}
  TableEntry& operator[](const String& s);//{return r[t->col[s]];}
};

class TableIterator : public std::iterator<std::forward_iterator_tag, TableRow>{ // Table iterator
  Table* table;
  int row;
public:
  TableIterator(Table* t, int r = 0) : table(t), row(r)  {}
  TableRow* operator->(void);//{return &table[row];}
  TableRow& operator*(void);//{return table[row];}
  TableIterator& operator++(void);
  TableIterator operator++(int);
  
  bool operator==(const TableIterator& r){
    if(table != r.table) return false;
    if(row != r.row) return false;
    return true;
  }
  bool operator!=(const TableIterator& it){return !operator==(it);}
  //  TableEntry operator[](String& col);//{return table->T[row][col];}
};

ostream& operator<<(ostream& os, const TableRow& r);


Table csv_read(char* fname, int maxrecs = -1, int blocksize = 100);
class TableView;
class TableIndex;
class Table {
  friend class TableIterator;
  friend class TableRow;
  friend class TableView;
  friend class TableIndex;
  int _nrows; // no. of rows
  int _ncols; // no. of cols
  Array<TableRow> T;
  Index<String> col;
  Array<String>column_names;
public:
  Table(Array<String>names, int blocksize = 100);
  TableEntry& operator()(int i, String& c) {return T[i][col[c]];} // should have bounds checking option
  TableEntry& operator()(int i, int j){return T[i][j];}
  TableIterator begin(void){return TableIterator(this);}
  TableIterator end(void){return TableIterator(this,_nrows);}
  TableRow& operator[](int i){return T[i];}
  void add_col(String& name){col[name] = _ncols++;}
  void nrows(int nr){_nrows = nr;}
  int nrows(void){return _nrows;}
  int ncols(void){return _ncols;}
  const Index<String>& columns_by_name(void){return col;}
  const Array<String>& columns_by_number(void){return column_names;}
  TableView sort(const String& col_name);
  TableIndex index(const String& col_name);
  TableView search(
};

class TableView{
  Table* table;
  Array<uint> rows; // sorted list of row no.s
  int col_no;
  void reheap(int i, int n);
  
public:
  TableView(Table* t) : table(t), rows(t->nrows()){
    int n = t->nrows();
    for(int i = 0;i < n;i++)rows[i] = i;
  }
  TableView(Table* t, Callback* query);
  TableRow& operator[](int i){return (*table)[rows[i]];}
  void sort(const String& col_name);
  int nrows(void){return rows.nrows();}
};

class TableIndex{
  Table* table;
  Index<TableEntry> idx;
  int col_no;
public:
  TableIndex(Table* t, const String& col_name);
  TableRow& operator[](const TableEntry& e){return table->T[idx[e]];}
  bool has(const TableEntry& e){return idx.has(e);}
};
  



#endif
