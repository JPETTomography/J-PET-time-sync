#ifndef _________DELTAS_H_______
#       define _________DELTAS_H_______
#include <JPetHit/JPetHit.h>
#include "PetDict.h"
const size_t thresholds_count=4;
struct TOT_cut{
  double A[thresholds_count],B[thresholds_count];
};
inline std::istream&operator>>(std::istream&str,TOT_cut&item){
  for(size_t t=0;t<thresholds_count;t++)str>>item.A[t];
  for(size_t t=0;t<thresholds_count;t++)str>>item.B[t];
  return str;
}
inline std::ostream&operator<<(std::ostream&str,const TOT_cut&item){
  for(size_t t=0;t<thresholds_count;t++)str<<item.A[t]<<" ";
  for(size_t t=0;t<thresholds_count;t++)str<<item.B[t]<<" ";
  return str;
}

#endif