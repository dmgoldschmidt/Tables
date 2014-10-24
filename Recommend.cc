/* Outline for recommender system:
We'll do the simplest case in which there are just two possible votes $m_{ij} = +1 if user $i$ liked movie $j$,
$m_{ij} = -1$ if user $i$ disliked movie $j$, and $m_{ij} = 0$ if user $i$ has not seen movie $j$.

1. Sort the sparse matrix in place by row, then copy and re-sort by column.  Then create a list of indices for the 
row-sorted copy where the index marks the start of the row.  Do the same for the column-sorted copy.  Now we can 
sequentially access any row or col of the matrix.

2. Now fix a movie $j$ and form a dense (mostly zero) vector of the jth column, and pre-multiply by the transpose 
of the sparse matrix to get a vector $B_j$. The $k^{th}$  entry of $B_j$ is $b_{kj} := a_{kj} - d_{kj}$, ie. 
agreements - disagreements between columns $k$ and $j$. Now do the same multiply, but ignoring signs to get a 
vector $C_j$.  The $k^{th}$ entry of $C_j$ is $c_{kj} := a_{kj} + d_{kj}$, ie. agreements + disagreements.  The pair 
$(B_j,C_j)$ measures how similar movie $j$ is to all the other movies. We'll call it a "similarity pair". 

3. For a recommender system, we fix a user $i$.  We want to find those movies $j$ which $i$ has not seen, but which 
are most similar to the movies $i$ likes, and most dissimilar to those he dislikes. First compute similarity pairs  
$(B_k,C_k)$ for all movies $k$ seen by user $i$, i.e. with $m_{ik} \neq 0$, and form $\beta := \sum_km_{ik}B_k$ and 
$\gamma := \sum_kC_k$.  
Then $\beta_j$ is the total number of agreements-disagreements over all users between movie $j$ and  movies user $i$ 
liked, plus total disagreements minus agreements between movie $j$ and movies user $i$ disliked.  Thus 
$b_j := \beta_j/\gamma_j$ is the bulge in favor of the hypothesis that user $i$ will like movie $j$.  We can sort 
the $b_j$ to get a list of recommendations for user $i$.

4. To estimate uncertainty due to sample size, we can do Bayesian updates using a Beta-prior:  Let $p_{kj}$ be the 
probability that the votes of a random user who has seen both movies $k$ and $j$ will agree. Let the $j^{th}$ entry 
of $B_k$ be $B_{kj}$.  So $p_{kj} = Pr(B_{kj} = 1). Suppose we have a prior Beta distribution on $p = p_{kj}$ with parameters $b_0$,$b_1$.  So $Pr(p) \propto p^{b_0-1}(1-p)^{b_1-1}$.  
So if we begin with a flat prior (b_0=b_1=1) and  observe a total of $a$ agreements and $d$ disagreements, the 
posterior is $\propto p^a(1-p)^d$, so the mean is $p_0 := E(p) = a/(a+d)$ and the standard deviation is
$\sigma = \sqrt(p_0(1-p_0)/(a+d+1))$. 

5. But the above is too naive.  Namely, the fact that user $i$ already likes two very similar movies $j_1$ and
$j_2$ should not provide independent evidence that he will like another movie which is similar to both of them.  
So we should have a correction term based on the similarity of $j_1$ and $j_2$. 
*/


#include "Matrix.h"
#include "Awk.h"
#include "GetOpt.h"
#include "Heap.h"
#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

Recommender::Recommender(const matrix& A){

  SparseMatrix S(++maxrow,++maxcol,by_rows); // initialize the sparse matrix
  if(user >= maxrow){
    cerr <<"No user "<<user<<". Bailing out.\n";
    exit(1);
  }
  if(verbose)cout <<"Sparse matrix is "<< maxrow <<" x "<<maxcol<<" with "<< i << " nonzero entries."<<endl;
  S.transpose();
  Array<int> col_starts = S.sort();
  matrix by_cols = A.copy(); // sorted by cols
  if(verbose)cout << "col sort:\n"<<by_cols.slice(0,0,10,3);
  if(verbose)cout << "col_starts:\n"<<col_starts<<endl;
  S.transpose();
  Array<int> row_starts = S.sort();

  if(verbose)cout <<"row sort:\n"<<by_rows.slice(0,0,10,3);
  if(verbose)cout <<"row_starts:\n"<<row_starts<<endl;
  b_vector.reset(S.ncols(),1,0); // total agreements - disagreements
  c_vector.reset(S.ncols(),1,0); // total agreements + disagreements
  similarity.reset(S.ncols(),1); // agr. - disagr. for a particular movie
  dense_col.reset(S.nrows(),1); // temp column vector
}
const Heap<IndexPair<double> >&  Recommender::operator()(int user, int nrec){
  if(row_starts[user] == -1){
    cerr<<"No entries for user "<<user<<endl;
    exit(1);
  }

  S.transpose(); // so that S*dense_col entry k is the inner product of col k with dense_col
  for(i = row_starts[user];by_rows(i,0) == user;i++){ //
    
    int j = by_rows(i,1); // next movie rated by user
    dense_col.zero(); // zero the temp dense vector
    for(int k = col_starts[j];by_cols(k,1) == j;k++) dense_col(by_cols(k,0),0) = by_rows(i,2)*by_cols(k,2);
    similarity = S*dense_col;
    if(verbose)cout << format("similarity vector for user %d, movie %d:\n",user,j)<<similarity<<endl;
    b_vector += similarity;
    c_vector += S.abs_mult(dense_col);
  }    
  if(verbose)cout << "b_vector:\n"<<b_vector;
  if(verbose)cout << "\nc_vector:\n"<<c_vector;
  if(verbose)cout << "bulge for user "<<user<<": ";
  for(int i = 0;i < b_vector.nrows();i++) if(verbose)cout << format("%d: %f\n",i,b_vector(i,0)/c_vector(i,0));
  IndexPair<double> pair;
  topn.reset(nrec); // reset the heap
  
  for(int i = 0;i < b_vector.nrows();i++){ 
    //    if(verbose)cout << format("%d:%f(%d) ",i,b_vector(i,0)/c_vector(i,0),(int)c_vector(i,0));
    bool skip = false;
    for(int j = row_starts[user];by_rows(j,0) == user;j++){ 
      if(by_rows(j,1) == i){ // we've seen this one
        skip = true;
        break;
      }
    }
    if(!skip){
      if(verbose)cout << "adding "<<i<<endl;
      topn.add(IndexPair<double>(i,b_vector(i,0)/c_vector(i,0)));
      if(verbose)topn.dump(0);
    }
  }
    
  cout << "top "<<nrec<<" recommendations for user "<<user<<":\n";
  for(int i = 0;i < nrec;i++){
    cout << format("movie %d (pr %f, #votes %.0f)\n", topn[0].i, (topn[0].item + 1)/2, c_vector(topn[0].i,0));
    topn.pop();
    if(verbose)topn.dump(0);
  }
}
