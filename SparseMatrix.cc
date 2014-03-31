#include "Matrix.h"
#include <cstdlib>
SparseMatrix::SparseMatrix(int m, int n, const matrix& A0) : _nrows(m), _ncols(n), _nentries(A0.nrows()), A(A0), transposed(0) {
    if(A.ncols() != 3) throw "SparseMatrix: input must have three columns\n";
  }

matrix SparseMatrix::operator*(const matrix& M){ // premultiply M by sparse matrix
  int m = transposed? _ncols:_nrows;
  int n = transposed? _nrows:_ncols;
  if(n != M.nrows())throw "SparseMatrix premultiply: dimension error\n";
  double zero = 0; // hokey! need another Matrix constructor
  matrix M1(m,M.ncols(),&zero);
  //cout << format("premultiply sparse (%dx%d)*(%dx%d)\n",m,n,M.nrows(),M.ncols());  
  for(int k = 0;k < _nentries;k++){
    //    cout << format("entry %d: i = %d, j = %d, %f\n",k,(int)A(k,transposed),(int)A(k,transposed^1),A(k,2));
    int i = (int)A(k,transposed);
    int j = (int)A(k,transposed^1);
    for(int l = 0;l < M.ncols();l++){
      M1(i,l) += A(k,2)*M(j,l);
      //      cout << format("ans(%d,%d) += A(%d,%d)*M(%d,%d) = %f*%f\n",i,l,i,j,j,l,A(k,2),M(j,l));
    }
  }
  return M1;
}

matrix SparseMatrix::svd(int r, int niters, double eps, unsigned seed){
   
  matrix A(_nrows+_ncols+1,r); // holds singular values in row_0, right singular vectors in col.s 1 -> r,  rows 1 -> ncols
  //  left singular vectors in col.s 1 -> r,  rows ncols+1 -> ncols+nrows
  matrix S = A.slice(0,0,1,r); // singular values
  matrix U = A.slice(1,0,_ncols,r); // right singular vectors
  matrix V = A.slice(1+_ncols,0,_nrows,r); // left singular vectors [ V^t(*this)U = diag(S)]
  
  int iter = 0;
  double error =  1.0;
  
  srandom(seed);
  double rand_max = (double)RAND_MAX;
  for(int i = 1;i < A.nrows();i++){ // initialize to random values uniform on (0,1)
    for(int j = 0;j < A.ncols();j++)A(i,j) = random()/rand_max;
  }

  while(iter < niters && error > eps){
    //cout << format("this is %d x %d, U is %d x %d\n",_nrows,_ncols,U.nrows(),U.ncols());
    V.copy((*this)*U);
    //    cout << "\nfirst print:\nS:\n"<<S<<"\nU:\n"<<U<<"\nV:\n"<<V;
    error = gram_schmidt(V,S);
    //   cout << "\nsecond print.  Error = "<<error<<"\nS:\n"<<S<<"\nU:\n"<<U<<"\nV:\n"<<V;
    transpose();
    U.copy((*this)*V);
    //   cout << "\nthird print:\nS:\n"<<S<<"\nU:\n"<<U<<"\nV:\n"<<V;
    transpose();
    double error1 = gram_schmidt(U,S);
    //    cout << "\nfourth print.  Error = "<<error1<<":\nS:\n"<<S<<"\nU:\n"<<U<<"\nV:\n"<<V;
    error = (error+error1)/2;
    cout << "iteration "<<iter<<" error: "<<error<<"\n******\n";
    iter++;
  }
  return A;
}

int SparseMatrix::row(int m) const {return A(m,transposed);}
int SparseMatrix::col(int m) const {return A(m,transposed^1);}
double SparseMatrix::entry(int m) const{return A(m,2);}
   
int SparseMatrix::nrows(void) const {return transposed? _ncols:_nrows;}
int SparseMatrix::ncols(void) const {return transposed? _nrows:_ncols;}
int SparseMatrix::nentries(void) const {return _nentries;}
