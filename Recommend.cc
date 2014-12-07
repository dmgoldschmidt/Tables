#include "Matrix.h"
#include "Awk.h"
#include "GetOpt.h"
#include "Heap.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

Recommender::Recommender(const matrix& A){
  int maxrow = 0;
  int maxcol = 0;
  for(int i = 0;i < A.nrows();i++){
    if(A(i,0) > maxrow)maxrow = A(i,0); // get dimensions
    if(A(i,1) > maxcol)maxcol = A(i,1);
  }
  S.reset(++maxrow,++maxcol,A); // initialize the sparse matrix
  row_starts = S.sort();
  by_rows = Entries.copy(); // save copy of Entries sorted by row
  if(verbose)cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< A.nrows() << " nonzero entries."<<endl;
  if(verbose)cout <<"row sort:\n"<<by_rows.slice(0,0,10,3);
  if(verbose)cout <<"row_starts:\n"<<row_starts<<endl;
  // now sort by columns
  S.transpose();
  col_starts = S.sort();
  // Entries is now sorted by cols first, then rows 
  if(verbose)cout << "col sort:\n"<<by_cols.slice(0,0,10,3);
  if(verbose)cout << "col_starts:\n"<<col_starts<<endl;
  S.transpose(); // untranspose the matrix

  //initialize working vectors
  b_vector.reset(S.ncols(),1,0); // total agreements - disagreements
  c_vector.reset(S.ncols(),1,0); // total agreements + disagreements
  similarity.reset(S.ncols(),1); // agr. - disagr. for a particular movie
  dense_col.reset(S.nrows(),1); // temp column vector
}
double Recommender::prob(int user, int m){ // prob. user will like movie m

  int sim = 0;
  double scnt = 0;
  for(int i = row_starts[user];by_rows(i,0) == user;i++){ //
    int j = by_rows(i,1); // next movie rated by user
    int l = col_starts[m]; // Entries(l,0) is a user who rated movie m
    for(int k = col_starts[j];Entries(k,1) == j && Entries(l,1) == m;k++){ // Entries(k,0) is a user who rated movie j
      while(Entries(l,1) == m){
        if( Entries(l,0) == Entries(k,0)){
          sim += Entries(m,2)*Entries(k,2)*by_rows(i,2); 
          scnt++;
          break;
        }
        l++;
      }
    }
  }
  return sim/scnt;
}

