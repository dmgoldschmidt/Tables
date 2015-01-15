#include "Recommender.h"
#include "Matrix.h"
#include "Awk.h"
#include "GetOpt.h"
#include "Heap.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

Recommender::Recommender(const matrix& A, bool v) : verbose(v){
  int maxrow = 0;
  int maxcol = 0;
  for(int i = 0;i < A.nrows();i++){
    if(A(i,0) > maxrow)maxrow = A(i,0); // get dimensions
    if(A(i,1) > maxcol)maxcol = A(i,1);
  }
  S.reset(++maxrow,++maxcol,A); // initialize the sparse matrix
  row_starts = S.sort();
  by_rows = S.Entries.copy(); // save copy of Entries sorted by row
  if(verbose)cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< A.nrows() << " nonzero entries."<<endl;
  if(verbose)cout <<"row sort:\n"<<by_rows.slice(0,0,10,3);
  //  if(verbose)cout <<"row_starts:\n"<<row_starts<<endl;
  // now sort by columns
  S.transpose();
  col_starts = S.sort();
  by_cols = S.Entries.copy();
  // Entries is now sorted by cols first, then rows 
  if(verbose)cout << "col sort:\n"<<S.Entries.slice(0,0,10,3);
  //  if(verbose)cout << "col_starts:\n"<<col_starts<<endl;
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
  double inc;
  for(int i = row_starts[user];by_rows(i,0) == user;i++){
    int j = by_rows(i,1); // next movie rated by user
    int l = col_starts[m]; // by_cols(l,0) is now the lowest numbered user who rated movie m
    for(int k = col_starts[j];by_cols(k,1) == j && by_cols(l,1) == m;k++){ // by_cols(k,0) is a user who rated movie j
      for(;by_cols(l,1)==m && by_cols(l,0) < by_cols(k,0);l++); // Did he also rate movie m?
      if(by_cols(l,1) != m) break; // out of movie m raters.  On to the next movie rated by our user
      if( by_cols(l,0) == by_cols(k,0)){ // OK, this user rated both movies 
        sim += (inc = by_cols(l,2)*by_cols(k,2)*by_rows(i,2)); // inc = +-1 unless the entry has been zeroed out
        scnt += inc*inc; // in case it's been set to zero
      }
    }
  }
  //  cout << "prob: "<<(1+sim/scnt)/2<<" #votes: "<<scnt<<endl;

  return scnt? (1+sim/scnt)/2 : .5;
}
bool Recommender::get_entry(int i, int j, double& x){
  int r = row_starts[i];
  while(by_rows(r,0) == i && by_rows(r,1) != j) r++;
  if(by_rows(r,0) != i || by_rows(r,1) != j) return false;
  x = by_rows(r,2);
  return true;
}
void Recommender::set_entry(int i, int j, double x){
  int r = row_starts[i];
  while(by_rows(r,0) == i && by_rows(r,1) != j) r++;
  if(by_rows(r,1) != j) return;
  by_rows(r,2) = x;
  int c = col_starts[j];
  while(by_cols(c,1) == j && by_cols(c,0) != i) c++;
  if(by_cols(c,1) != j || by_cols(c,0) != i) return;
  by_cols(c,2) = x;
}
