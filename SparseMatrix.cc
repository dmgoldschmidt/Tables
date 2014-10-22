#include "Matrix.h"
#include <cstdlib>

// NOTE:  SparseMatrix needs to be converted to a template, to properly handle both int and double matrices

SparseMatrix::SparseMatrix(int m, int n, const matrix& A) : _nrows(m), _ncols(n), _nentries(A.nrows()), Entries(A), transposed(0) {
    if(Entries.ncols() != 3) throw "SparseMatrix: input must have three columns\n";
  }

void SparseMatrix::swap_rows(int i1, int i2){
  double temp;
  for(int j = 0;j < 3;j++){
    temp = Entries(i1,j);
    Entries(i1,j) = Entries(i2,j);
    Entries(i2,j) = temp;
  }
}
      

void SparseMatrix::reheap(int i, int n) {// the children of this sub-heap are heaps.  Make it a heap recursively
  int i1 = 2*i+1;
  if(i1 >=n) return; 
  int i2 = i1+1;
  
  if(i2 >= n || Entries(i2,transposed) <  Entries(i1,transposed)) i2 = i1; // choose larger child
  if(Entries(i,transposed) < Entries(i2,transposed)){ // we need parent >= larger child
    swap_rows(i,i2);
    reheap(i2,n); // recursively fix up child heap
  }
}

Array<int> SparseMatrix::sort(void){ // sort entries by row. (Set transpose flag to do col.s) 
  // Return array of row starts
  int n = nentries();
  for(int i = n/2;i >= 0;i--)reheap(i,n); // make initial heap
  while(n > 1){ // swap first and last row index and reheap
    swap_rows(0,--n);
    reheap(0,n);
  }
  //  cout <<"Entries:\n"<<Entries;
  // OK, Entries is now sorted by row
  int row = -1;
  Array<int> starts(nrows(),&row); // array of starting row indices.  Initialize to -1 i.e. row absent
  // starts[i] is the first Entry for row i 
  for(int i = 0;i < nentries();i++){
    if((int)Entries(i,transposed) != row){// new row
      row = (int)Entries(i,transposed);
      starts[row] = i;
    }
  }
  return starts;
}  

matrix SparseMatrix::operator*(const matrix& M){ // premultiply M by sparse matrix
  int m = transposed? _ncols:_nrows;
  int n = transposed? _nrows:_ncols;
  if(n != M.nrows())throw format("SparseMatrix premultiply: dimension error (n = %d, M.nrows = %d)\n",n,M.nrows());
  double zero = 0; // hokey! need another Matrix constructor
  matrix M1(m,M.ncols(),&zero);
  //cout << format("premultiply sparse (%dx%d)*(%dx%d)\n",m,n,M.nrows(),M.ncols());  
  for(int k = 0;k < _nentries;k++){
    //    cout << format("entry %d: i = %d, j = %d, %f\n",k,(int)A(k,transposed),(int)A(k,transposed^1),A(k,2));
    int i = (int)Entries(k,transposed);
    int j = (int)Entries(k,transposed^1);
    for(int l = 0;l < M.ncols();l++){
      M1(i,l) += Entries(k,2)*M(j,l);
    }
  }
  return M1;
}

matrix SparseMatrix::abs_mult(const matrix& M){ // premultiply M by sparse matrix, ignore all signs
  int m = transposed? _ncols:_nrows;
  int n = transposed? _nrows:_ncols;
  if(n != M.nrows())throw format("SparseMatrix abs_multiply: dimension error (n = %d, M.nrows = %d)\n",n,M.nrows());
  double zero = 0; // hokey! need another Matrix constructor
  matrix M1(m,M.ncols(),&zero);
  //cout << format("premultiply sparse (%dx%d)*(%dx%d)\n",m,n,M.nrows(),M.ncols());  
  for(int k = 0;k < _nentries;k++){
    //    cout << format("entry %d: i = %d, j = %d, %f\n",k,(int)A(k,transposed),(int)A(k,transposed^1),A(k,2));
    int i = (int)Entries(k,transposed);
    int j = (int)Entries(k,transposed^1);
    for(int l = 0;l < M.ncols();l++){
      M1(i,l) += fabs(Entries(k,2))*fabs(M(j,l));
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
    V.copy((*this)*U); // V <- (*this)*U)
    //    cout << "\nfirst print:\nS:\n"<<S<<"\nU:\n"<<U<<"\nV:\n"<<V;
    error = gram_schmidt(V,S);
    //   cout << "\nsecond print.  Error = "<<error<<"\nS:\n"<<S<<"\nU:\n"<<U<<"\nV:\n"<<V;
    transpose();
    U.copy((*this)*V);// U <- (*this)^T*V
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
matrix SparseMatrix::svd1(int& r, int niters, double eps, unsigned seed){ // note: r may be reset to effective rank
   
  matrix A(_nrows+_ncols+1,r); // holds singular values in row_0, right singular vectors in col.s 0 -> r-1,  
  // rows 1 -> ncols,  left singular vectors in col.s 0 -> r-1,  rows ncols+1 -> ncols+nrows
  matrix S = A.slice(0,0,1,r); // singular values
  matrix U = A.slice(1,0,_ncols,r); // right singular vectors
  matrix V = A.slice(1+_ncols,0,_nrows,r); // left singular vectors [ V^t(*this)U = diag(S)]

  srandom(seed);
  double rand_max = (double)RAND_MAX;
  for(int i = 1;i < _ncols;i++){ // initialize right sing. vectors to random values uniform on (-1,1)
    for(int j = 0;j < r;j++)A(i,j) = 2*random()/rand_max - 1;
  }
  
  for(int i = 0;i < r;i++){ // get the ith singular vector
    cout << "Computing singular vector "<<i<<endl;
    matrix U_i = A.slice(1,i,_ncols,1); // ith col of U
    matrix V_i = A.slice(1+_ncols,i,_nrows,1); // ith col of V
    cout << "dim(U_i) =  "<< U_i.nrows()<<", dim(V_i) = "<<V_i.nrows()<<endl;
    cout << "nrows = "<<nrows()<<", ncols = "<<ncols()<<endl;
  
    matrix U_last(_ncols,1); // previous values
    matrix delta(_ncols,1);

    for(int j = 0;j < U_i.nrows();j++) U_i(j,0) = 2*random()/rand_max - 1; // initialize U_i to random
    double err = eps;
    bool done = false;
    for(int iter = 0;!done && iter < niters && err >= eps;iter++) {
      for(int j = 0;j < i;j++){ //orthogonalize against previous U's
        double scale = dot_cols(U,i,j);
        for(int k = 0;k < U_i.nrows();k++) U_i(k,0) -= scale*U(k,j); //make U_i orthogonal to U_j
      }
      double length = sqrt(dot_cols(U_i,0,0));
      for(int k = 0;k < U_i.nrows();k++) U_i(k,0) /= length; // normalize
      U_last.copy(U_i); // save previous value
      V_i.copy((*this)*U_i); // V_i <- (*this)*U_i
      length = sqrt(dot_cols(V_i,0,0));
      for(int k = 0;k < V_i.nrows();k++) V_i(k,0) /= length; // normalize
      transpose();
      U_i.copy((*this)*V_i); // U_i <- (*this)^t*V_i
      S(0,i) = sqrt(dot_cols(U_i,0,0));
      for(int k = 0;k < U_i.nrows();k++) U_i(k,0) /= S(0,i); // normalize
      //      cout << "iteration "<<iter<<", U_i:\n"<<U_i<<"U_last:\n"<<U_last<<"V_i:\n"<<V_i<<"S:\n"<<S;
      if(S(0,i) < eps){ // null sing. vector
        done = true;
        r = i; // we only got i sing. vectors
        continue; 
      }
      transpose();
      delta = U_i - U_last;
      err = sqrt(dot_cols(delta,0,0)/delta.nrows());
      if(iter%10 == 0){
        cout << "Singular values:\n"<< S;
        cout <<"end iteration "<<iter<<", error = "<<err<<endl;
      }
    }
  }      
  return A;
}

int SparseMatrix::row(int m) const {return Entries(m,transposed);}
int SparseMatrix::col(int m) const {return Entries(m,transposed^1);}
double SparseMatrix::entry(int m) const{return Entries(m,2);}
   
int SparseMatrix::nrows(void) const {return transposed? _ncols:_nrows;}
int SparseMatrix::ncols(void) const {return transposed? _nrows:_ncols;}
int SparseMatrix::nentries(void) const {return _nentries;}
