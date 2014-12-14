#ifndef RECOMMEND_H
#define RECOMMEND_H
#include "Heap.h"
#include "Matrix.h"

class Recommender {
  SparseMatrix S;
  matrix by_rows;
  matrix by_cols;
  Array<int> row_starts;
  Array<int> col_starts;
  matrix b_vector; // total agreements - disagreements
  matrix c_vector; // total agreements + disagreements
  matrix similarity; // agr. - disagr. for a particular movie
  matrix dense_col; // temp column vector
  Recommender(const Recommender& R); // no copying
  Recommender& operator=(const Recommender& R);
  bool verbose;
public:
  Recommender(const matrix& A, bool v = false);
  ~Recommender(void){};
  const Heap<IndexPair<double> >& operator()(int user, int nrec); //return top nrec recommendations for user <user>. 
  double prob(int i, int j); // compute prob.(user i will like movie j) 
};



#endif
