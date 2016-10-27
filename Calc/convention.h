#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
#include <j-pet-framework-extension/PetDict.h>
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
template<class DataType>
inline const std::shared_ptr<JPetMap<DataType>> make_JPetMap(){
  return std::shared_ptr<JPetMap<DataType>>(new JPetMap<DataType>({48,48,96}));
}
struct SyncAB_results{
  MathTemplates::value<double>peak;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncAB_results&item){
  return str>>item.peak>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncAB_results&item){
  return str<<item.peak<<"\t"<<item.chi_sq;
}
struct SyncOposite_results{
  MathTemplates::value<double>peak;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncOposite_results&item){
  return str>>item.peak>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncOposite_results&item){
  return str<<item.peak<<"\t"<<item.chi_sq;
}
template<class DataType>
inline const std::shared_ptr<JPetMap<DataType>> make_half_JPetMap(){
  return std::shared_ptr<JPetMap<DataType>>(new JPetMap<DataType>({24,24,48}));
}

const size_t neighbour_delta_id=3;
struct SyncNeighbour_results{
  MathTemplates::value<double>left,right,assymetry;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncNeighbour_results&item){
  return str>>item.left>>item.right>>item.assymetry>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncNeighbour_results&item){
  return str<<item.left<<"\t"<<item.right<<"\t"<<item.assymetry<<"\t"<<item.chi_sq;
}
struct DeltaT_results{
  MathTemplates::value<double> A,B;
};
inline std::istream&operator>>(std::istream&str,DeltaT_results&item){
  return str>>item.A>>item.B;
}
inline std::ostream&operator<<(std::ostream&str,const DeltaT_results&item){
  return str<<item.A<<"\t"<<item.B;
}
#endif