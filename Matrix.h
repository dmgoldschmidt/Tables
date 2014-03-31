#ifndef MATRIX_H
#define MATRIX_H
#include "Array.h"
#include "util.h"

// #define COL_STRIDE (((int*)Base::buf)[2])
// #define MDATA ((SCALAR*)(Base::buf+3*sizeof(int)/sizeof(SCALAR) + 1))

#define ENTRY (operator())

template<typename SCALAR>
class Matrix;
typedef Matrix<double> matrix;

class SparseMatrix {
  const matrix& A; // A(m,0) = row index i, A(m,1) = col index j, A(m,2) = (i,j)-entry 
  int _nrows;
  int _ncols;
  int _nentries; // no. of tabulated entries = no. of rows of A 
  int transposed;
public:
  SparseMatrix(int m, int n, const matrix& A0);
  matrix operator*(const matrix& M); // premultiply dense matrix M by sparse matrix *this
  matrix svd(int r, int niters = 100, double eps = 1.0e-8, unsigned seed = 1);
  int row(int m) const;
  int col(int m) const;
  double entry(int m) const;
   
  int nrows(void) const;
  int ncols(void) const;
  int nentries(void) const;
  void transpose(void){transposed ^= 1;} // flip transposed flag
  void set_transpose_flag(bool b){transposed = (int)b;}
};

struct Givens{
  double sin_t;
  double cos_t;
  double h;
  double eps;

  Givens(double e = 1.0e-5) : eps(e){}
  
  bool reset(double& a0, double& b0){
    if(fabs(b0) < eps) return false;
    double a = a0*a0;
    double b = b0*b0;
    h = fabs(b < a? a0*sqrt(1+b/a) : b0*sqrt(1+a/b)); // numerical hygene for sqrt(a+b)
    cos_t = a0/h;
    sin_t = b0/h;
    a0 = h;
    b0 = 0;
    //    cout << "sin_t = "<<sin_t<<", cos_t = "<<cos_t<<endl;
    return true;
  }
   
  inline void rotate(double& x, double& y){
    double z = x;
    x = cos_t*z + sin_t*y;
    y = -sin_t*z + cos_t*y;
  }
};

template<typename SCALAR>
class Matrix : public Array<SCALAR> {
  typedef Array<SCALAR> Base;
  int offset; // these parameters support strides
  int _nrows;
  int _ncols;
  int col_stride;
public:
  Matrix(void) : Base(), offset(0), _nrows(0), _ncols(0), col_stride(0) {}
  //  Matrix(int m, int n) : Base(m*n), offset(0), _nrows(m), _ncols(n), col_stride(n) {}
  Matrix(int m, int n, const SCALAR* fill = nullptr ): Base(m*n,fill), offset(0), _nrows(m), _ncols(n), col_stride(n) {
    
    }

  Matrix(initializer_list<Array<SCALAR>> l){
    _nrows = l.size();
    _ncols = 0;
    const Array<SCALAR>* p = l.begin();
    
    for(int i = 0;i < _nrows;i++){
      if(p[i].len() > _ncols)_ncols = p[i].len();
    }
    col_stride = _ncols;
    offset = 0;
    Base::reset(_nrows*_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < p[i].len();j++)ENTRY(i,j) = p[i][j];
    }
  };
 
  Matrix(const Matrix& M) : Base(M), offset(M.offset), _nrows(M._nrows), _ncols(M._ncols), col_stride(M.col_stride){} // shallow copy
  
  // void reset(int m, int n){
  //   Base::reset(m*n);
  //   offset = 0;
  //   _nrows = m;
  //   col_stride = _ncols = n;
    
  // }
  void reset(int m, int n, const SCALAR* fill = nullptr){
    Base::reset(m*n,fill);
    offset = 0;
    _nrows = m;
    col_stride = _ncols = n;
  }
  Matrix copy(int new_cols = 0) const{ // return a new deep copy
                                       // with optional extra cols
    Matrix M(_nrows,_ncols+new_cols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++) M(i,j) = ENTRY(i,j);
    }
    return M;
  }
  void copy(const Matrix& A){ // deep copy A to *this
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++) ENTRY(i,j) = A(i,j);
    }
  }
  Matrix slice(int i, int j, int m, int n) const{ // shallow copy mxn submatrix at (i,j)
    Matrix M(*this);
    M.offset = i*col_stride+j;
    M._nrows = m;
    M._ncols = n;
    return M;
  }
    
  SCALAR& operator()(int i, int j) const{return Base::operator[](offset + i*col_stride + j);}
  int nrows(void) const {return _nrows;}
  int ncols(void) const {return _ncols;}

  //*********

  Matrix operator+(const Matrix& A) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = ENTRY(i,j)+A(i,j);
    }
    return B;
  }
  Matrix operator-(const Matrix& A) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = ENTRY(i,j)-A(i,j);
    }
    return B;
  }
  Matrix operator*(const Matrix& A)const {
    Matrix B(_nrows,A._ncols);

    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < A._ncols;j++){
        B(i,j) = 0;
        for(int k = 0;k < _ncols;k++){
          B(i,j) += ENTRY(i,k)*A(k,j);
          //          cout << format("ENTRY(%d,%d) = %d, A(%d,%d) = %d, B(%d,%d) = %d\n",i,k,ENTRY(i,k),k,j,A(k,j),i,j,B(i,j));
        }
      }
    }
    return B;
  }
  Matrix operator*(const SparseMatrix& A){ // post-multiply by sparse matrix A // NOTE: only for SCALAR = double
    double zero = 0;
    if(_ncols != A.nrows()) throw "Sparse post-multiply dimension error\n";
    Matrix B(_nrows,A.ncols(),&zero);
    for(int m = 0;m < A.nentries();m++){
      int j = A.row(m);
      int k = A.col(m);
      for(int i = 0;i < _nrows;i++){ 
	B(i,k) += ENTRY(i,j)*A.entry(m);
	//	cout << format("B(%d,%d) = %f\n",i,k,B(i,k));
      }
    }
    return B;
  }

  Matrix operator-(void) const {
    Matrix B(_nrows,_ncols);
    for(int i = 0;i < _nrows;i++){
      for(int j = 0;j < _ncols;j++)B(i,j) = -ENTRY(i,j);
    }
    return B;
  }
  Matrix T(void) const {
    Matrix B(_ncols,_nrows);
    for(int i = 0;i < _ncols;i++){
      for(int j = 0;j < _nrows;j++)B(i,j) = ENTRY(j,i);
    }
    return B;
  }
        
          
 





#ifdef DEBUG
  void dump(void) const{
    std::cout << "nrows = "<<_nrows<<", ncols = "<<_ncols<<", offset = "<<offset<<std::endl<<
      "refcount = "<< Base::first->refcount <<", col stride = "<<col_stride<<std::endl;
    std::cout << *this;
  }
#endif

}; // end Matrix template

template<typename SCALAR>
std::ostream& operator <<(std::ostream& os, const Matrix<SCALAR>& M){
  for(int i = 0;i < M.nrows();i++){
    for(int j = 0;j < M.ncols();j++)os << M(i,j)<<" ";
    os <<"\n";
  }
  return os;
}

// declarations for matrix (=Matrix<double>)

double dot_cols(const matrix& A, int i, int j);
double gram_schmidt(matrix& A, matrix& S, double eps = 1.0e-8);
int ut0(matrix& A, double eps = 1.0e-10); // upper-triangularize in place by Givens row rotations
int ut(matrix& A, double eps = 1.0e-10); // upper-triangularize in place by Givens row rotations
matrix qr(const matrix& A);
void reduce(matrix& A, double eps = 1.0e-10); // row-reduce upper-triangular A in place
void solve(matrix& A, double eps = 1.0e-10); // solve linear equations in-place 
matrix inv(const matrix& A, double eps = 1.0e-10);
Array<matrix> svd(const matrix& A, double eps = 1.0e-10, int maxiters = 10);
struct Svd{
  int ncols;
  int nrows;
  matrix AUV;
  matrix AU;
  matrix AV;
  matrix U;
  matrix V;
  matrix A;

  void reduce(const matrix& A0);
};
Array<matrix> svd1(const matrix& A, double eps = 1.0e-10, int maxiters = 10);



 

#endif
