#include "Matrix.h"
#include "util.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char** argv){
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
    Svd R;
    R.reduce(A);
    cout << "dense SVD:\n"<<R.AUV;
    SparseMatrix S(m,n,B);
    Array<int> starts = S.sort();
    cout <<"Sorted B:\n"<<B;
    for(int i = 0;i < m;i++){
      cout << format("row %d: %d\n",i,starts[i]);
    }
    exit(0);
    S.transpose();
    cout << "A = "<<A;
    cout << "A^t*A = "<<A.T()*A;
    cout << "A*A^t = "<<A*A.T();
    cout << "S^t*A = "<<S*A;
    cout << "A*S^t = "<<A*S;
    ut(A);
    cout << "ut(A):\n"<<A;
    int nvecs = 3;
    S.transpose();
    matrix C0 = S.svd(nvecs,10);
    cout <<"C0:\n"<<C0<<"*****\n";
      
    matrix C = S.svd1(nvecs,100);
    cout << "C:\n"<<C;
  }
  catch(string s){
    cout << s;
  }
  catch(const char* s){
    cout << s;
  }
}
