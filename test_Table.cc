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
  cout << "nrows: "<<t.nrows()<<endl;//" ndbls: "<<t.ndbl << " nstr: "<< t.nstr<<endl;
  //  cout << "Doubles:\n"<<t.Doubles<<"Strings:\n"<<t.Strings<<endl;
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
  // for(auto it = t.columns.begin(); it != t.columns.end(); it++){
  //   cout << *it << endl;
  // }
  // cout <<"****** begin auto& : syntax\n\n";
  // for(auto& it : t.columns){
  //   cout << it << endl;
  // }


  Table t1 = csv_read("../data",10);
  String left("left");
  String right("right");
  String row_no("row_no");
  t1.add_col(row_no,1);
  for(int i = 0;i < t1.nrows();i++){
    t1(i,row_no) = i;
  }
  int nprint = 0;
  for(auto& row : t1){
    
    if(nprint++ < 10){
      cout << "left: "<<row[left]<<"  right: " <<row[right]<<" row_no: "<<row[row_no]<<endl;
    }
  }
}
