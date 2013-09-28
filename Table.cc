#include "Table.h"
#include "Awk.h"


int type(const char* str){ // get type of string field
  char* endptr;
  strtod(str,&endptr);
  return *endptr == '\0';
}
int type(const String& s){
  return type(s.c_str());
}

void hex_dump(char* s){
  while(*s) cout <<format("%x ",*s++);
  cout << endl;
}

void printable(char* p){ // delete non-printable characters
  for(char* q = p;*q;q++){ 
    if(*q >= 32 && *q < 127){ //copy it
      *p++ = *q;
    }
  }
  *p = '\0';
}

Table csv_read(char* fname, int maxrecs, int blocksize){ 
  /* read comma-separated values into a Table.
   * first line has the column names
   */

  Awk reader(','); // initialize to read csv lines
  if(!reader.open(fname)){
    cerr<< "Can't open "<<fname<<endl;
    exit(1);
  }
  int nfields = reader.next(); // read header line
  cout << "header line: "<<reader.line<<" nfields = "<<nfields<<endl;
  
  Array<String> names(nfields);
  for(int i = 0;i < nfields;i++){
    printable(reader.field(i)); // delete non-printable characters
    names[i] = reader.field(i);
    if(names[i] == ""){
      cerr << format("csv_read: field %d must have a name\n",i);
      exit(1);
    }
  }
  Table T(names,blocksize);

  int nrecs = 0;
  while(int nf = reader.next()){
    if(maxrecs >= 0 && nrecs >= maxrecs)break;
    if(nf != nfields){
      cerr << format("csv_read: record %d has %d fields (should be %d)\n",nrecs,nf,nfields);
      exit(1);
    }

    for(int i = 0;i < nfields;i++){
      printable(reader.field(i));
      String field = reader.field(i);
      if(field != "" && field != "NULL" && field != "MISSING"){
	int field_type = type(field);
	if(field_type == 0)T(nrecs,i) = field;
	else T(nrecs,i) = atof(field.c_str());
      }
    }
    nrecs++;
  }
  reader.close();
  T.nrows(nrecs);
  cout << format("csv_read: read %d records\n",nrecs); 
  return T;
}

#define MIN_ARRAY_SIZE 10
Table::Table(Array<String>names, int blocksize) : column_names(names) {
  _nrows = _ncols = 0;
  int size = max<int>(MIN_ARRAY_SIZE,blocksize); // in case blocksize is too small

  for(int i = 0;i < names.len();i++){
    add_col(names[i]);
    cout << "added column "<<names[i]<<endl;
  }
  T.reset(size);
  for(int i = 0;i < size;i++){
    T[i].reset(size);
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
TableEntry TableRow::operator[](String& c){return table->T[row][table->col[c]];}

ostream& operator<<(ostream& os, const TableEntry& x){
  if(x.type == dble) os << x.d;
  else if (x.type == Str) os << x.s;
  else os << null;
  return os;
}

ostream& operator<<(ostream& os, TableRow& r){
  const Index<String>& col = r.table->columns_by_name();
  for(IndexIterator<String> p = col.begin();p != col.end(); p++){
    //    cout << "iterator current item: "<<p.current_item<<":  ";
    os << p->key<<": "<<r[p->key]<<" ";
  }
  os <<endl;
  return os;
}
