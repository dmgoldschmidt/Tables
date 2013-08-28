#include "Table.h"
#include "Awk.h"


int type(char* str){ // get type of string field
  char* endptr;
  strtod(str,&endptr);
  return endptr != str;
}

Table csv_read(char* fname, int nrecs){
  Awk reader(',');
  if(!reader.open(fname)){
    cerr<< "Can't open "<<fname<<endl;
    exit(1);
  }
  reader.next(); // read header line
  int nfields = reader.nf;
  //  cout << "header line: "<<reader.line<<" nfields = "<<nfields<<endl;
  
  Array<String> names(nfields);
  Array<int> types(nfields);

  for(int i = 0;i < nfields;i++){
    names[i] = reader.field(i);
    //    cout << format("names[%d]: %s\n",i,names[i].c_str());
  }
  
  reader.next(); // read first data line and deduce types
  for(int i = 0;i < nfields;i++){
    types[i] = type(reader.field(i));
    //    cout << format("types[%d]: %d\n",i,types[i]);
  }
  Table t(nrecs,names,types);
  for(int i = 0;i < nrecs;i++){
    for(int j = 0;j < nfields;j++){
	t(i,names[j]) = reader.field(j);
	//	cout << format("t(%d,%s) = %s\n ",i,names[j].c_str(),t(i,names[j]).c_str());
    }
    reader.next();
  }
  reader.close();
  return t;
}
#define MIN_ARRAY_SIZE 10
Table::Table(int nr,Array<String>names,Array<int>types){
  _nrows = nr;
  nstr = ndbl = 0;

  for(int i = 0;i < names.len();i++) add_col(names[i], types[i]);
  Doubles.reset(_nrows);
  Strings.reset(_nrows);
  for(int i = 0;i < _nrows;i++){
    Doubles[i].reset(max<int>(MIN_ARRAY_SIZE, ndbl));
    Strings[i].reset(max<int>(MIN_ARRAY_SIZE, nstr));
  }
}

void Table::add_col(String& name, int type){
  if(type == 0) columns[name] = Pair<int>(nstr++,type);
  else columns[name] = Pair<int>(ndbl++,type);
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

ostream& operator<<(ostream& os, const TableEntry& x){
  if(x.d != nullptr) os << *x.d;
  else if (x.s != nullptr) os << *x.s;
  else throw "Object type error\n";
  return os;
}
