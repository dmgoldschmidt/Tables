#include "Matrix.h"

SparseMatrix::SparseMatrix(int m, int n, const matrix& A0) : _nrows(m), _ncols(n), _nentries(A0.nrows()), A(A0), transposed(0) {
    if(A.ncols() != 3) throw "SparseMatrix: input must have three columns\n";
  }

matrix SparseMatrix::operator*(const matrix& M){ // premultiply M by sparse matrix
  int m = transposed? _ncols:_nrows;
  int n = transposed? _nrows:_ncols;
  if(n != M.nrows())throw "SparseMatrix premultiply: dimension error\n";
  double zero = 0; // hokey! need another Matrix constructor
  matrix M1(m,M.ncols(),&zero);
  // cout << "transposed: "<<transposed<<" transposed^1: "<<(transposed^1)<<endl;
  //cout <<"A:\n"<<A;
  
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
int SparseMatrix::row(int m) const {return A(m,transposed);}
int SparseMatrix::col(int m) const {return A(m,transposed^1);}
double SparseMatrix::entry(int m) const{return A(m,2);}
   
int SparseMatrix::nrows(void) const {return transposed? _ncols:_nrows;}
int SparseMatrix::ncols(void) const {return transposed? _nrows:_ncols;}
int SparseMatrix::nentries(void) const {return _nentries;}
