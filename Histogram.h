#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include "Array.h"
using namespace std;

template <typename T>
class Histogram {
  const Array<T> bin; // user inputs bin boundaries
  // bucket[0] : item < bin[0], bucket[i]: bin[i-1]<= item < bin[i], bucket[n]: item >= bin[n-1] 
  int ncnts; // total no. of counts
  int nbins; // length of bin array
  int search(T item, int lower, int upper){ // recursive binary search -- needs bin[lower] <= item < bin[upper]
    if(upper - lower <= 1) return lower;
    int mid = (lower+upper)/2;
    if(item < bin[mid])return search(item,lower,mid);
    else return search(item,mid,upper);
public:
  Array<int> counts;
  Histogram(void) : n(0) {}
  Histogram(const Array<T>& b) {reset(b);}
  void reset(const Array<T>& b){
    ncnts = 0;
    nbins = b.len();
    bin = b;
    int zero = 0;
    counts.reset(nbins + 1,&zero);
  }
  void add(T item){
    if(item < bin[0])counts[0]++;
    else if(item >= bin[nbins-1])counts[nbins]++;
    else counts[search(item,0,nbins)]++;
    ncnts++;
  }
  int nentries(void){return ncnts;}
};
  



#endif
