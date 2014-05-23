/* Outline for recommender system:
We'll do the simplest case in which there are just two possible votes (+1,-1).
1. Sort sparse matrix in place by row, then copy and re-sort by column.  Then create a list of indices for the row-sorted copy where the index marks the start of the row.  Do the same for the column-sorted copy.  Now we can sequentially access any row or col of the matrix.

2. We want to predict the (i,j) entry $m_{ij}$ of the matrix from all the non-zero entries.  Form a dense (mostly zero) vector of the jth column, and pre-multiply by the transpose of the sparse matrix. The $k^{th}$ entry is $b_{kj} := a_{kj} - d_{kj}$, ie. agreements - disagreements between columns $k$ and $j$. Now do the same multiply, but ignoring signs.  The $k^{th}$ entry here is $c_{kj} := a_{kj}+d_{kj}$, ie. agreements + disagreements.  These two vectors measure how similar movie j is to all the other movies.  
Now form the sums $B_{ij} := \sum_{k\neq j} m_{ki}b_{kj}$ and $C_{ij} := \sum_{k\neq j} m_{ki}c_{kj}$.  
Then $B_{ij}/C_{ij}$ is the bulge in favor of $m_{ij} = +1$. 
*/


#include "Matrix.h"
#include "Awk.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char** argv){
  int nvec = atoi(argv[3]);
  int nrows = atoi(argv[2]);
  Awk reader('\t');
  reader.open(argv[1]);
  matrix A(nrows,3,0);
  matrix by_rows;
  int i = 0;
  int maxrow = 0;
  int maxcol = 0;
  while(reader.next() && i < nrows){
    A(i,0) = strtod(reader.field(0),NULL); // row index
    A(i,1) = strtod(reader.field(1),NULL); // col index
    A(i,2) = strtod(reader.field(2),NULL); // value
    if(A(i,0) > maxrow)maxrow = A(i,0); // get dimensions
    if(A(i,1) > maxcol)maxcol = A(i,1);
    i++;
  }
  if(i < nrows) by_rows = A.slice(0,0,i,3);
  else by_rows = A;
  //  cout << B;

  SparseMatrix S(maxrow,maxcol,by_rows); // initialize the sparse matrix
  cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< i << " nonzero entries."<<endl;
  


  matrix C;
  try{
    C = S.svd1(nvec,100);
  }
  catch(char const* s){
    cout << "Caught: "<<s;
    exit(1);
  }
 
}
#ifdef MORE
    
  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int seed = atoi(argv[3]);
  matrix A(m,n);
  matrix B(m*n,3);
  int k = 0;
  srandom(seed);
  double rand_max = (double)RAND_MAX;
  // for(int i = 1;i < A.nrows();i++){ // initialize to random values uniform on (0,1)
  //   for(int j = 0;j < A.ncols();j++)A(i,j) = random()/rand_max;
  // }


  for(int i = 0;i < A.nrows();i++){
    for(int j = 0;j < A.ncols();j++){
      A(i,j) = random()/rand_max;
      B(k,0) = i;
      B(k,1) = j;
      B(k++,2) = A(i,j);
    }
  }
  try{
    SparseMatrix S(4,3,B);
    S.transpose();
    cout << "A = "<<A;
    cout << "A^t*A = "<<A.T()*A;
    cout << "A*A^t = "<<A*A.T();
    cout << "S^t*A = "<<S*A;
    cout << "A*S^t = "<<A*S;
    ut(A);
    cout << "ut(A):\n"<<A;
    matrix C = S.svd(3,100);
    cout << "C:\n"<<C;
  }
  catch(char const* s){
    cout << s;
  }
}
#endif
