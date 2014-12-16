#include "Matrix.h"
#include "Awk.h"
#include "GetOpt.h"
#include "Heap.h"
#include "Recommender.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;


int main(int argc, char** argv){
  string data = string("ml_data");
  int nlines = 100000; // defaults to all data
  int user = -1; // no user specified
  int movie = -1; // no movie specified
  int nrec = 5; // default to 5 recommendations
  bool score_existing = false;
  bool dense_print = false;
  bool verbose = false;
  char* help_msg = "Usage:\n\
-data <filename>: read data from <filename>\n\
-nlines <n>: max. no. of lines to read from <filename>\n\
-user <u>: make recommendations for user <u>\n\
-nrec <r>: make <r> recommendations\n\
-score: score existing votes against user model\n\
-dense_print: print entire matrix in dense form\n";
 
  GetOpt cl(argc,argv,help_msg); // parse command line

  cl.get("data",data); // preferences file (line format: user \t movie \t rating)
  cl.get("nlines",nlines); // #lines to read from data
  cl.get("user",user); // user no. for whom to make recommendations
  cl.get("nrec",nrec); // no. of recommendations requested
  cl.get("movie",movie);
  if(cl.get("score")) score_existing = true; // score existing choices for this user
  if(cl.get("dense_print")) dense_print = true;
  if(cl.get("verbose")) verbose = true;

  // if(user == -1){
  //   cerr<<"No user specified.  Please rerun with -user <u> set.\n";
  //   exit(1);
  // }

  Awk reader; // separator is space or tab
  if(!reader.open(data.c_str())){
    cerr<<"Can't open "<<data<<endl;
    exit(1);
  }
  matrix A(nlines,3,0);
  int i = 0;
  while(reader.next() && i < nlines){ // read it in
    A(i,0) = strtod(reader.field(0),NULL); // row index
    A(i,1) = strtod(reader.field(1),NULL); // col index
    A(i,2) = strtod(reader.field(2),NULL); // value
    if(A(i,2) > 3) A(i,2) = 1; // 4 or 5 == like
    else A(i,2) = -1; // 1,2,3 == not so hot
    i++;
  }
  if(i < nlines) A = A.slice(0,0,i,3); // if we didn't get as many entries as we thought

  Recommender R(A,false);
  cout << format("prob user %d likes movie %d: %f\n",user,movie,R.prob(user,movie));
}
