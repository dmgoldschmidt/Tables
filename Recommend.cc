#include "Matrix.h"
#include "Awk.h"
#include "GetOpt.h"
#include "Heap.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

Recommender::Recommender(const matrix& A){

  SparseMatrix S(++maxrow,++maxcol,by_rows); // initialize the sparse matrix
  if(user >= maxrow){
    cerr <<"No user "<<user<<". Bailing out.\n";
    exit(1);
  }
  if(verbose)cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< i << " nonzero entries."<<endl;
  S.transpose();
  Array<int> col_starts = S.sort();
  matrix by_cols = A.copy(); // sorted by cols
  if(verbose)cout << "col sort:\n"<<by_cols.slice(0,0,10,3);
  if(verbose)cout << "col_starts:\n"<<col_starts<<endl;
  S.transpose();
  Array<int> row_starts = S.sort();

  if(row_starts[user] == -1){
    cerr<<"No entries for user "<<user<<endl;
    exit(1);
  }
  if(verbose)cout <<"row sort:\n"<<by_rows.slice(0,0,10,3);
  if(verbose)cout <<"row_starts:\n"<<row_starts<<endl;
  matrix b_vector(S.ncols(),1,0); // total agreements - disagreements
  matrix c_vector(S.ncols(),1,0); // total agreements + disagreements
  matrix similarity(S.ncols(),1); // agr. - disagr. for a particular movie
  matrix dense_col(S.nrows(),1); // temp column vector

  // int counts[10];
  
  // for(i = 0;i < 10;i++) counts[i] = 0;
  // for(i = 0;i < A.nrows();i++) counts[(int)A(i,2)]++;
  // for(i = 0;i < 10;i++) if(verbose)cout << i<<":"<<counts[i] <<" ";
  // if(verbose)cout <<endl;
  // exit(0);
  S.transpose(); // so that S*dense_col entry k is the inner product of col k with dense_col
  for(i = row_starts[user];by_rows(i,0) == user;i++){ //
    
    int j = by_rows(i,1); // next movie rated by user
    dense_col.zero(); // zero the temp dense vector
    for(int k = col_starts[j];by_cols(k,1) == j;k++) dense_col(by_cols(k,0),0) = by_rows(i,2)*by_cols(k,2);
    similarity = S*dense_col;
    if(verbose)cout << format("similarity vector for user %d, movie %d:\n",user,j)<<similarity<<endl;
    b_vector += similarity;
    c_vector += S.abs_mult(dense_col);
  }    
  if(verbose)cout << "b_vector:\n"<<b_vector;
  if(verbose)cout << "\nc_vector:\n"<<c_vector;
  if(verbose)cout << "bulge for user "<<user<<": ";
  for(int i = 0;i < b_vector.nrows();i++) if(verbose)cout << format("%d: %f\n",i,b_vector(i,0)/c_vector(i,0));
  IndexPair<double> pair;
  Heap<IndexPair<double> > topn(nrec);
  
  for(int i = 0;i < b_vector.nrows();i++){ 
    //    if(verbose)cout << format("%d:%f(%d) ",i,b_vector(i,0)/c_vector(i,0),(int)c_vector(i,0));
    bool skip = false;
    for(int j = row_starts[user];by_rows(j,0) == user;j++){ 
      if(by_rows(j,1) == i){ // we've seen this one
        skip = true;
        break;
      }
    }
    if(!skip){
      if(verbose)cout << "adding "<<i<<endl;
      topn.add(IndexPair<double>(i,b_vector(i,0)/c_vector(i,0)));
      if(verbose)topn.dump(0);
    }
  }
    
  cout << "top "<<nrec<<" recommendations for user "<<user<<":\n";
  for(int i = 0;i < nrec;i++){
    cout << format("movie %d (pr %f, #votes %.0f)\n", topn[0].i, (topn[0].item + 1)/2, c_vector(topn[0].i,0));
    topn.pop();
    if(verbose)topn.dump(0);
  }
}
