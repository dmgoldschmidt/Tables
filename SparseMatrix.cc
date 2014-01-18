#include "Matrix.h"

SparseMatrix::SparseMatrix(int m, int n, const matrix& A0) : _nrows(m), _ncols(n), _nentries(A0.nrows()), A(A0) {
    if(A.ncols() != 3) throw "SparseMatrix: input must have three columns\n";
  }

matrix SparseMatrix::operator*(const matrix& M){ // premultiply M by sparse matrix
  if(_ncols != M.nrows())throw "SparseMatrix premultiply: dimension error\n";
  double zero = 0;
  matrix M1(_nrows,M.ncols(),&zero);
  
  for(int m = 0;m < _nentries;m++){
    int i = A(m,0);
    int j = A(m,1);
    for(int k = 0;k < M.ncols();k++) M1(i,k) += A(m,2)*M(j,k);
  }
  return M1;
}
 int SparseMatrix::row(int m) const {return A(m,0);}
 int SparseMatrix::col(int m) const {return A(m,1);}
 double SparseMatrix::entry(int m) const{return A(m,2);}
   
 int SparseMatrix::nrows(void) const {return _nrows;}
 int SparseMatrix::ncols(void) const {return _ncols;}
 int SparseMatrix::nentries(void) const {return _nentries;}
