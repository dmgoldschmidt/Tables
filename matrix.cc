#include "Matrix.h"
#include "util.h"

// matrix routines for SCALAR=double

int ut0(matrix& A, double eps = 1.0e-10){// upper-triangularize in place by Givens row rotations
  int nrot = 0;
  double cos_t, sin_t, h, a,b, tmp;
  for(int i = 1;i < A.nrows();i++){
    for(int j = 0;j < i;j++){
      if(A(i,j) > A(j,j)){ // numerical hygene for h = sqrt(A(i,j)^2 + A(j,j)^2
        a = A(i,j);
        b = A(j,j);
      }
      else{
        a = A(j,j);
        b = A(i,j);
      }
      if(a < eps) continue;
      h = a*sqrt(1+b*b/(a*a));
      nrot++;
      sin_t = A(j,j)/h;
      cos_t = A(i,j)/h;
      A(j,j) = h;
      A(i,j) = 0;
      for(int k = j+1;k < A.ncols();k++){
        tmp = sin_t*A(j,k) + cos_t*A(i,k);
        A(i,k) = -cos_t*A(j,k) + sin_t*A(i,k);
        A(j,k) = tmp;
      }
    }
  }
  return nrot;
}

int ut(matrix& A, double eps = 1.0e-10){// upper-triangularize in place by Givens row rotations
  int nrot = 0;
  Givens R;

  for(int i = 1;i < A.nrows();i++){
    for(int j = 0;j < i;j++){
      if(!R.reset(A(j,j),A(i,j))) continue;
      nrot++;
      for(int k = j+1;k < A.ncols();k++){
	R.rotate(A(j,k),A(i,k));
      }
    }
  }
  return nrot;
}



matrix qr(const matrix& A){
  matrix QR(A.nrows(),A.nrows()+A.ncols(),0);
  matrix Q(QR.slice(0,A.ncols(),A.nrows(),A.nrows()));
  matrix R(QR.slice(0,0,A.nrows(),A.ncols()));

  R.copy(A);
  for(int i = 0;i < Q.nrows();i++)Q(i,i) = 1.0; // initialize Q to I
  ut(QR);
  return QR;
}


void reduce(matrix& A, double eps = 1.0e-10){ // row-reduce upper-triangular A in place
  int n = A.nrows();
  double a,b;

  for(int i = 0;i < n;i++){
    if( (b = A(i,i)) < eps)throw "solve: Matrix is singular\n";
    A(i,i) = 1.0;
    for(int j = i+1;j < A.ncols();j++) A(i,j) /= b;
    for(int k = 0;k < i;k++){
      a = A(k,i);
      A(k,i) = 0;
      for(int j = i+1;j < A.ncols();j++) A(k,j) -= a*A(i,j);
      //      cout << format("\nk,i = (%d,%d) A:\n",k,i)<<A;
    }
  }
}

void solve(matrix& A, double eps = 1.0e-10){ // solve linear equations in-place 
  // A is an m x (m+k) matrix of m equations (in m unknows) with k right-hand sides
  ut(A); // rotate  to upper-triangular form;
  reduce(A,eps); // now row-reduce coefficients to identity.  Each rhs is now solved
}

matrix inv(const matrix& A, double eps = 1.0e-10){
  matrix QR = qr(A); // rotate to upper-triangular form
  // and append the rotation matrix (as extra columns)
  int n = QR.nrows();

  reduce(QR,eps); // now row-reduce
  return QR.slice(0,n,n,n); // return the left-most n columns
}


Array<matrix> svd(const matrix& A, double eps = 1.0e-10, int maxiters = 10){
  Array<matrix> QR(2);
  Array<matrix> R(2);
  int j,niters,n = std::min(A.nrows(),A.ncols());
  double delta;

  QR[0] = qr(A);
  R[0] = QR[0].slice(0,0,A.nrows(),A.ncols());
  QR[1] = qr(R[0].T());
  R[1] = QR[1].slice(0,0,A.ncols(),A.nrows());
  j = 0;
  niters = 0;
  do{
    R[j].copy(R[1-j].T());
    ut(QR[j]);
    delta = 0;
    for(int i = 0;i < n;i++){
      if(QR[j](i,i) < eps)continue;
      delta += fabs(QR[j](i,i) - QR[1-j](i,i))/QR[j](i,i);
    }
    delta /= n;
    //    cout << "iteration "<<niters<<": delta = "<<delta<<endl;
    //    cout << format("R[%d]:\n",j)<<R[j];
    j = 1-j;
  }while(delta > eps && ++niters < maxiters);
  return QR;
}

struct Svd{
  int ncols;
  int nrows;
  matrix AUV;
  matrix AU;
  matrix AV;
  matrix U;
  matrix V;
  matrix A;

  void reduce(const matrix& A0){
    Givens R;

    nrows = A0.nrows();
    ncols = A0.ncols();
    AUV.reset(nrows+ncols,nrows+ncols,0);
    // define the submatrices
    AU = AUV.slice(0,0,nrows,ncols+nrows);
    AV = AUV.slice(0,0,nrows+ncols,ncols);
    U = AUV.slice(0,ncols,nrows,nrows);
    V = AUV.slice(nrows,0,ncols,ncols);
    A = AUV.slice(0,0,nrows,ncols);

    A.copy(A0); // copy in the input
    for(int i = 0;i < nrows;i++)U(i,i) = 1.0; // set rotation matrices = identity
    for(int i = 0;i < ncols;i++)V(i,i) = 1.0;
    ut(AU); // row rotate to upper triangular form 
    for(int i = 0;i < nrows-1;i++){ // zero the ith row above the super-diagonal with column rotations
      for(int j = ncols-1;j > i+1;j--){ // rotate columns j and j-1 to zero out AV(i,j)
	if(!R.reset(AV(i,j-1),AV(i,j)))continue;
	AV(i,j-1) = R.h; AV(i,j) = 0;
	for(int k = i+1;k < nrows+ncols;k++)
	  R.rotate(AV(k,j-1),AV(k,j));
	if(!R.reset(AU(j-1,j-1),AU(j,j-1)))continue;
	// remove resulting lower-triangular "residue" with a row rotation
	AU(j-1,j-1) = R.h;
	AU(j,j-1) = 0;
	for(int k = j;k < nrows+ncols;k++)
	  R.rotate(AU(j-1,k),AU(j,k));
      }
    }
    // OK, A is now upper triangular with only the first super-diagonal non-zero
    int  maxiters = 1000;
    bool doit,not_done;
    for(int niters = 0;niters < maxiters;niters++){
      not_done = false;
      for(int j = 0;j < ncols-1;j++){
	if( (doit = R.reset(A(j,j),A(j,j+1))) ){
	  for(int k = j+1;k < nrows+ncols;k++)
	    R.rotate(A(k,j),A(k,j+1));
	}
	not_done |= doit;
      }
      if(!not_done)break;
      not_done = false;
      for(int i = 0;i < ncols-1;i++){
	if( (doit = R.reset(A(i,i),A(i+1,i))) ){
	  for(int k = i+1;k < nrows+ncols;k++)
	    R.rotate(A(i,k),A(i+1,k));
	}
	not_done |= doit;
      }
      if(!not_done)break;
      //      cout <<"iteration "<<niters<<"\n"<<A;
    }
  }
};
    

Array<matrix> svd1(const matrix& A, double eps = 1.0e-10, int maxiters = 10){
  Array<matrix> QR(2);
  Array<matrix> R(2);
  int j,niters,n = std::min(A.nrows(),A.ncols());
  double delta;

  QR[0] = qr(A);
  R[0] = QR[0].slice(0,0,A.nrows(),A.ncols());
  QR[1] = qr(R[0].T());
  R[1] = QR[1].slice(0,0,A.ncols(),A.nrows());
  j = 0;
  niters = 0;
  do{
    R[j].copy(R[1-j].T());
    ut(QR[j]);
    delta = 0;
    for(int i = 0;i < n;i++){
      if(QR[j](i,i) < eps)continue;
      delta += fabs(QR[j](i,i) - QR[1-j](i,i))/QR[j](i,i);
    }
    delta /= n;
    //    cout << "iteration "<<niters<<": delta = "<<delta<<endl;
    //    cout << format("R[%d]:\n",j)<<R[j];
    j = 1-j;
  }while(delta > eps && ++niters < maxiters);
  return QR;
}

double dot_cols(const matrix& A, int i, int j){
  double ans = 0;
  for(int k = 0;k < A.nrows();k++) ans += A(k,i)*A(k,j);
  return ans;
}

int gram_schmidt(matrix& A, double eps){ // orthonormalize the columns of A
  int m = A.nrows();
  int n = A.ncols();

  for(int i = 0;i < n;){
    // inductively, col.s 0,1,...,i-1 are already orthonormal
    for(int j = 0;j < i;j++){
      double scale = dot_cols(A,i,j);
      for(int k = 0;k < m;k++)A(k,i) -= scale*A(k,j); // A_i -= <A_i,A_j>A_j
      // OK, col. i is now orthogonal to cols 0,1,...,j
    } 
    double len = sqrt(dot_cols(A,i,i));
    if(len < eps){// swap with last col and reduce n
      double temp;
      for(int k = 0;k < m;k++){
	temp = A(k,i);
	A(k,i) = A(k,n-1);
	A(i,n-1) = temp;
      }
      n--;
      continue;
    }
    for(int k = 0;k < m;k++) A(k,i) /= len; // normalize col i
    i++;
  }
  return n;
}
    
  