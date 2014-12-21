#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include "Array.h"
#include <iostream>
using namespace std;

template <typename T>
class Histogram {
public:  //  friend std::ostream& operator <<(std::ostream& os, Histogram<T>& H);
  Array<T> bin; // user inputs bin boundaries
  // bucket[0] : item < bin[0], bucket[i]: bin[i-1]<= item < bin[i], bucket[n]: item >= bin[n-1] 
  int ncnts; // total no. of counts
  int nbins; // length of bin array
  int search(T item, int lower, int upper){ // recursive binary search -- needs bin[lower] <= item < bin[upper]
    if(upper - lower <= 1) return upper;
    int mid = (lower+upper)/2;
    if(item < bin[mid])return search(item,lower,mid);
    else return search(item,mid,upper);
  }
  //public:
  Array<int> counts;
  Array<int> hits;
  Histogram(void) : nbins(0) {}
  Histogram(const Array<T>& b) {reset(b);}
  void reset(const Array<T>& b){
    ncnts = 0;
    nbins = b.len();
    bin = b;
    int zero = 0;
    counts.reset(nbins + 1,&zero);
    hits.reset(nbins + 1,&zero);
  }
  void add(T item, bool hit = false){
    int bin_no;
    if(nbins == 0) throw "Histogram: class is unitialized.\n";
    if(item < bin[0])bin_no = 0;
    else if(item >= bin[nbins-1])bin_no = nbins;
    else bin_no = search(item,0,nbins-1);
    ncnts++;
    counts[bin_no]++;
    if(hit)hits[bin_no]++;
  }
  int nentries(void){return ncnts;}
};
  
template<typename T>
ostream& operator <<(ostream& os, Histogram<T>& H){
  os << format("<%.2f\t%d/%d\t\t%.2f\n",H.bin[0],H.hits[0],H.counts[0],
               H.counts[0]!=0? H.hits[0]/(double)H.counts[0]:0);
  for(int i = 1;i < H.nbins;i++){
    os << format("%.2f-%.2f\t%d/%d\t\t%.2f\n",H.bin[i-1],H.bin[i],H.hits[i],H.counts[i],
               H.counts[i]!=0? H.hits[i]/(double)H.counts[i]:0);
  }
  os << format(">%.2f\t%d/%d\t\t%.2f\n",H.bin[H.nbins-1],H.hits[H.nbins],H.counts[H.nbins],
               H.counts[H.nbins]!=0? H.hits[H.nbins]/(double)H.counts[H.nbins]:0);
  return os;
} 



#endif
