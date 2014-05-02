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
  matrix B;
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
  if(i < nrows) B = A.slice(0,0,i,3);
  else B = A;
  //  cout << B;

  SparseMatrix S(maxrow,maxcol,B);
  cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< i << " nonzero entries."<<endl;

  matrix C;
  try{
    C = S.svd1(nvec,10);
  }
  catch(char const* s){
    cout << "Caught: "<<s;
    exit(1);
  }
  //cout << "C:\n"<<C;
 
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
