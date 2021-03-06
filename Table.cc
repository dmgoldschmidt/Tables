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
    //    cout << "added column "<<names[i]<<endl;
  }
  T.reset(size);
  for(int i = 0;i < size;i++){
    T[i].reset(this,size);
  }
}

bool TableEntry::operator<(const TableEntry& te) const {
  if( (int)_type == (int)missing)return false;
  if( (int)te._type == (int)missing)return true;
  if((int)_type != (int)te._type) throw "TableEntry::operator<: incomparable entries\n";
  switch(_type){
  case Str:
    return s < te.s;
  case dble:
    return d < te.d;
  }
}

bool TableEntry::operator==(const TableEntry& te) const {
  if(_type != te._type) return false;
  switch(_type){
  case missing: return true;
    break;
  case Str:
    return s == te.s;
  case dble:
    return d == te.d;
  }
}

TableIterator& TableIterator::operator++(void) { // ++it 
  if(++row > table->_nrows)row = table->_nrows;
  return *this;
}

TableIterator TableIterator::operator++(int){ // it++
  TableIterator invalue(*this); // return initial state 
  if(++row > table->_nrows)row = table->_nrows;
  return invalue;
}
//TableEntry TableIterator::operator[](String& c){return table->T[row][table->col[c]];}

TableRow* TableIterator::operator->(void){return &(*table)[row];}
TableRow& TableIterator::operator*(void){return (*table)[row];}

ostream& operator<<(ostream& os, const TableEntry& x){
  switch(x._type){
  case dble:
    os << x.d;
    break;
  case Str:
    os << x.s;
    break;
  case missing:
    os << null;
    break;
  }
  return os;
}

ostream& operator<<(ostream& os, TableRow& r){
  const Index<String>& col = r.t->columns_by_name();
  for(IndexIterator<String> p = col.begin();p != col.end(); p++){
    //    cout << "iterator current item: "<<p.current_item<<":  ";
    os << p->key<<": "<<r[p->key]<<" ";
  }
  os <<endl;
  return os;
}

TableView Table::sort(const String& col_name){
  TableView tv(this);
  tv.sort(col_name);
  return tv;
}

TableIndex Table::index(const String& col_name){
  TableIndex ti(this,col_name);
  return ti;
}

inline TableEntry& TableRow::operator[](const String& s){
  return r[t->col[s]];
}

void swap(uint& x, uint& y){
  uint t = x;
  x = y;
  y = t;
}

void TableView::reheap(int i, int n) {// the children of this sub-heap are heaps.  Make it a heap recursively
  int i1 = 2*i+1;
  if(i1 >=n) return; 
  int i2 = i1+1;
  Table& t = *table;
  
  if(i2 >= n || t(rows[i2],col_no) <  t(rows[i1],col_no)) i2 = i1; // choose larger child
  if(t(rows[i],col_no) < t(rows[i2],col_no)){ // we need parent >= larger child
    swap(rows[i],rows[i2]);
    reheap(i2,n); // recursively fix up child heap
  }
}

void TableView::sort(const String& col_name){
  auto item = table->col.find(col_name);
  if(item == nullptr) throw String("TableView::sort: no column ") + col_name;
  col_no = item->value;
  int n = table->nrows();
  for(int i = n/2;i >= 0;i--)reheap(i,n); // make initial heap
  while(n > 1){ // swap first and last row index and reheap
    swap(rows[0],rows[--n]);
    reheap(0,n);
  }
}  

TableIndex::TableIndex(Table* t, const String& col_name): table(t), idx(2*t->nrows()){
  auto item = table->col.find(col_name);
  if(item == nullptr) throw String("TableView::sort: no column ") + col_name;
  col_no = item->value;
  Table& tbl = *t;
  for(int i = 0;i < t->nrows();i++) idx[tbl[i][col_no]] = i;
}

    
