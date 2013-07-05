#include <iostream>
#include <cstdlib>
#include "Table.h"
#include "Awk.h"
#include "Index.h"
using namespace std;

// include <sstream>
// #include <string>

// double string2double( const std::string& a )
//   {
//   double result;
//   std::string s( a );

//   // Get rid of any trailing whitespace
//   s.erase( s.find_last_not_of( " \f\n\r\t\v" ) + 1 );

//   // Read it into the target type
//   std::istringstream ss( s );
//   ss >> result;

//   // Check to see that there is nothing left over
//   if (!ss.eof())
//     throw 1;

//   return result;
//   }

int type(char* str){ // get type of string field
   char* endptr;
  strtod(str,&endptr);
  return endptr != str;
}

Table csv_read(char* fname, int nrecs = 100){
  Awk reader(',');
  if(!reader.open(fname)){
    cerr<< "Can't open "<<fname<<endl;
    exit(1);
  }
  reader.next(); // read header line
  int nfields = reader.nf;
  cout << "header line: "<<reader.line<<" nfields = "<<nfields<<endl;
  
  Array<String> names(nfields);
  Array<int> types(nfields);

  for(int i = 0;i < nfields;i++){
    names[i] = reader.field(i);
    cout << format("names[%d]: %s\n",i,names[i].c_str());
  }
  
  reader.next(); // read first data line and deduce types
  for(int i = 0;i < nfields;i++){
    types[i] = type(reader.field(i));
    cout << format("types[%d]: %d\n",i,types[i]);
  }
  Table t(nrecs,names,types);
  for(int i = 0;i < nrecs;i++){
    for(int j = 0;j < nfields;j++){
	t(i,names[j]) = reader.field(j);
	cout << format("t(%d,%s) = %s\n ",i,names[j].c_str(),t(i,names[j]).c_str());
    }
    reader.next();
  }
  reader.close();
  return t;
}

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

  Array<String> names(2); // column names
  Array<int> types(2); // column types
  names[0] = "name";
  names[1] = "age";
  types[0] = 0; // String
  types[1] = 1; // Double
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
    cout <<"my name is "<<myname<<", my age is "<<myage<<" and my other age is "<<myage1<<endl;
  }
  cout << "our combined age is "<< t(0,age)+t(1,age)+t(2,age)<<endl;
  //  KeyValue<int,int> kv_test(1,2);
  //  cout << "kv_test: "<<kv_test<<endl;
  int i = 0;
  for(auto it = t.columns.begin(); it != t.columns.end(); it++){
    cout << *it << endl;
  }
  cout <<"****** begin auto& : syntax\n\n";
  for(auto& it : t.columns){
    cout << it << endl;
  }


  Table t1 = csv_read("../data",10);
  String left("left");
  for(int i = 0;i < 10;i++){
    cout << "left: "<<t1(i,left)<<endl;
  }
}
