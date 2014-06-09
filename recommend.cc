/* Outline for recommender system:
We'll do the simplest case in which there are just two possible votes (+1,-1).
1. Sort the sparse matrix in place by row, then copy and re-sort by column.  Then create a list of indices for the 
row-sorted copy where the index marks the start of the row.  Do the same for the column-sorted copy.  Now we can 
sequentially access any row or col of the matrix.

2. Now fix a movie $j$ and form a dense (mostly zero) vector of the jth column, and pre-multiply by the transpose 
of the sparse matrix to get a 
vector $B_j$. The $k^{th}$  entry of $B_j$ is $b_{kj} := a_{kj} - d_{kj}$, ie. agreements - disagreements between 
columns $k$ and $j$. Now do the same multiply, but ignoring signs to get a vector $C_j$.  The $k^{th}$ entry of $C_j$ 
is $c_{kj} := a_{kj}+d_{kj}$, ie. agreements + disagreements.  The pair $B_j,C_j$ measures how similar movie j is to 
all the other movies. We'll call it a "similarity pair". 

3. For a recommender system, we fix a user $i$.  We want to find those movies $j$ which $i$ has not seen, but which 
are most similar to the movies $i$ has seen. First compute similarity pairs  $B_k,C_k$ for all $k$ with 
$m_{ik} \neq 0$, and form $\beta := \sum_km_{ik}B_k$ and $\gamma := \sum_kC_k$.  Then $\beta_j$ is the total number of 
agreements-disagreements over all users between movie $j$ and  movies user $i$ liked, plus disagreements minus agreements between movie $j$ and movies user $i$ disliked.  Thus $b_j := \beta_j/\gamma_j$ is the bulge in favor of the hypothesis
 that user $i$ will like movie $j$.  We can sort the $b_j$ to get a list of recommendations for user $i$.
*/


#include "Matrix.h"
#include "Awk.h"
#include "GetOpt.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

int main(int argc, char** argv){
  string data = string("ml_data");
  int nlines = 100000; // defaults to all data
  int user = -1; // no user specified
  int nrec = 5;
  bool score_existing = false;
  bool dense_print = false;
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
  if(cl.get("score")) score_existing = true; // score existing choices for this user
  if(cl.get("dense_print")) dense_print = true;


  if(user == -1){
    cerr<<"No user specified.  Please rerun with -user <u> set.\n";
    exit(1);
  }

  Awk reader; // separator is space or tab
  if(!reader.open(data.c_str())){
    cerr<<"Can't open "<<data<<endl;
    exit(1);
  }
  matrix A(nlines,3,0);
  int i = 0;
  int maxrow = 0;
  int maxcol = 0;
  while(reader.next() && i < nlines){ // read it in
    A(i,0) = strtod(reader.field(0),NULL); // row index
    A(i,1) = strtod(reader.field(1),NULL); // col index
    A(i,2) = strtod(reader.field(2),NULL); // value
    // if(A(i,2) > 3) A(i,2) = 1; // 4 or 5 == like
    // else A(i,2) = -1; // 1,2,3 == not so hot
    if(A(i,0) > maxrow)maxrow = A(i,0); // get dimensions
    if(A(i,1) > maxcol)maxcol = A(i,1);
    i++;
  }
  if(i < nlines) A = A.slice(0,0,i,3); // if we didn't get as many entries as we though
  matrix by_rows = A; // this will be sorted by rows
  //  cout << "as read:\n"<<A.slice(0,0,10,3); // print first ten lines
  SparseMatrix S(++maxrow,++maxcol,by_rows); // initialize the sparse matrix
  cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< i << " nonzero entries."<<endl;
  S.transpose();
  Array<int> col_starts = S.sort();
  matrix by_cols = A.copy(); // sorted by cols
  cout << "col sort:\n"<<by_cols.slice(0,0,10,3);
  cout << "col_starts:\n"<<col_starts<<endl;
  S.transpose();
  Array<int> row_starts = S.sort();

  if(row_starts[user] == -1){
    cerr<<"No entries for user "<<user<<endl;
    exit(1);
  }
  cout <<"row sort:\n"<<by_rows.slice(0,0,10,3);
  cout <<"row_starts:\n"<<row_starts<<endl;
  matrix b_vector(S.ncols(),1,0); // total agreements - disagreements
  matrix c_vector(S.ncols(),1,0); // total agreements + disagreements
  matrix similarity(S.ncols(),1); // agr. - disagr. for a particular movie
  matrix dense_col(S.nrows(),1); // temp column vector

  // int counts[10];
  
  // for(i = 0;i < 10;i++) counts[i] = 0;
  // for(i = 0;i < A.nrows();i++) counts[(int)A(i,2)]++;
  // for(i = 0;i < 10;i++) cout << i<<":"<<counts[i] <<" ";
  // cout <<endl;
  // exit(0);
  S.transpose(); // so that S*dense_col entry k is the inner product of col k with dense_col
  for(i = row_starts[user];by_rows(i,0) == user;i++){ //
    
    int j = by_rows(i,1); // next movie rated by user
    dense_col.zero(); // zero the temp dense vector
    for(int k = col_starts[j];by_cols(k,1) == j;k++) dense_col(by_cols(k,0),0) = by_rows(i,2)*by_cols(k,2);
    similarity = S*dense_col;
    cout << format("similarity vector for user %d, movie %d:\n",user,j)<<similarity<<endl;
    b_vector += similarity;
    c_vector += S.abs_mult(dense_col);
  }    
  cout << "b_vector:\n"<<b_vector;
  cout << "\nc_vector:\n"<<c_vector;
  cout << "bulge for user "<<user<<": ";
  for(int i = 0;i < b_vector.nrows();i++) 
    cout << format("%d:%f(%d) ",i,b_vector(i,0)/c_vector(i,0),(int)c_vector(i,0));
  cout <<endl;


}
