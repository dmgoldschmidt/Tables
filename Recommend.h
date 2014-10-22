#ifndef RECOMMEND_H
#define RECOMMEND_H
#include "Heap.h"
#include "Matrix.h"

class Recommender {
  matrix by_rows;
  matrix by_cols;
  Array<int> row_starts;
  Array<int> col_starts;
  Recommender(const Recommender& R); // no copying
  Recommender& operator=(const Recommender& R);
public:
  Recommender(const matrix& A);
  ~Recommender(void);
  const Heap<IndexPair<double> >& operator()(user, nrec); 
//return top nrec recommendations for user <user>. 
  double prob(int i, int j); // compute prob.(user i will like movie j) (user i may or may not have yet seen movie j)
};



#endif
