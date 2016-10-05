#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
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