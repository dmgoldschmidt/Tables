#ifndef TABLE_H
#define TABLE_H
#include <iostream>
#include "util.h"
#include "Index.h"
#include "Array.h"
#include <limits>

using namespace std;
static String null("NULL");
static double NaN = numeric_limits<double>::quiet_NaN();
typedef enum{missing,dble,Str} TableEntryType;
struct TableEntry {
  TableEntryType type;
  double d;
  String s;

  TableEntry(void) : type(missing) {}
  TableEntry(double& x) : type(dble), d(x) {} //set to double
  TableEntry(String& x) : type(Str), s(x) {} //set to String
  void operator()(double& x){d = x; type = dble;} //set existing to double
  void operator()(String& x){type = Str; s = x;} //set existing to String
  TableEntry& operator=(const String& ss){ //set existing to String
    s = ss;
    type = Str;
    return *this;
  }
  TableEntry& operator=(const double dd){ //set existing to double
    d = dd;
    type = dble;
    return *this;
  }
  
  operator double&(void) { //output to double
    if(type == dble) return d;
    return NaN;
  }

  operator String&(void) { //output to String
    if(type == Str) return s;
    return null;
  }

  const char* c_str(void){
    if(type == Str)return s.c_str();
    if(type = dble)return String(d).c_str();
    return null.c_str();
  }
};

ostream& operator<<(ostream& os, const TableEntry& x);

class Table;
class TableView;

class TableRow : public std::iterator<std::forward_iterator_tag, TableRow>{ // Table iterator
  friend ostream& operator<<(ostream& os, TableRow& r);
  Table* table;
  int row;
public:
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
  TableEntry operator[](String& col);//{return table->T[row][col];}
};

ostream& operator<<(ostream& os, const TableRow& r);

Table csv_read(char* fname, int maxrecs = -1, int blocksize = 100);
class Table {
  friend class TableRow;
  int _nrows; // no. of rows
  int _ncols; // no. of cols
  Array<Array<TableEntry>> T;
  Index<String> col;
  Array<String>column_names;
public:
  Table(Array<String>names, int blocksize = 100);
  TableEntry& operator()(int i, String& c) {return T[i][col[c]];} // should have bounds checking option
  TableEntry& operator()(int i, int j){return T[i][j];}
  TableRow begin(void){return TableRow(this);}
  TableRow end(void){return TableRow(this,_nrows);}
  void add_col(String& name){col[name] = _ncols++;}
  void nrows(int nr){_nrows = nr;}
  int nrows(void){return _nrows;}
  int ncols(void){return _ncols;}
  const Index<String>& columns_by_name(void){return col;}
  const Array<String>& columns_by_number(void){return column_names;}
};



#endif
