#include "Histogram.h"
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
  int nscores = 0; // no. of scores to compute
  int nbins = 10; // no. of histogram bins
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
  cl.get("nscores",nscores); 
  cl.get("nbins",nbins);
  if(cl.get("verbose")) verbose = true;
  if(nscores < 0)nscores = nlines; // score everything

  // if(user == -1){
  //   cerr<<"No user specified.  Please rerun with -user <u> set.\n";
  //   exit(1);
  // }
  cout << "nbins: "<< nbins<<" nscores: "<<nscores<<endl;
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
  Array<double> bins(nbins);
  double deltax = 1.0/nbins;
  for(i = 0;i < nbins;i++) bins[i] = (2*i+1)*deltax/2.0;
  Histogram<double> H(bins);
  
  double score = 0;
  double entry,pred_entry;
  double right_wrong = 0;
  cout << "begin scoring loop\n";
  for(i = 0;i < nscores;i++){ // score the first nscores entries of A
    int row = A(i,0);
    int col = A(i,1);
    bool gotit = R.get_entry(row,col,entry);
    if(!gotit || entry != A(i,2)){
      cerr << format("Error at (%d,%d). entry = %.0f, gotit = %d\n",row,col,entry,gotit);
      exit(1);
    }
    R.set_entry(row,col,0); // zero out the entry
    double prob = R.prob(row,col); // prob. user i likes movie j
    score += log(1+(2*prob-1)*entry); // 1+ log(p) if entry = 1, 1+log(1-p) if entry = -1
    pred_entry = prob >= .5? 1:-1;
    right_wrong += entry*pred_entry;
    H.add(prob,entry == 1); // update the histogram
    R.set_entry(row,col,entry); // restore entry
    //    cout << format("entry(%d,%d): %.0f prob: %.2f score: %.2f\n",row,col,entry,prob,score);
  }
  cout << "score: "<< score/(log(2)*nscores)<<" bits/bit\n"<<"counts: "<<H.counts<<"\nhits: "<<H.hits<<endl;
  cout << 50*(1+right_wrong/nscores)<<"% correct"<<endl;
  cout << H;
}
