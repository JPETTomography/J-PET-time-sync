#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
struct SyncAB_results{
  MathTemplates::value<double>position,width;double chi_sq;
  inline const MathTemplates::value<double> Range()const{return {position.val(),1.0*width.val()};}
};
inline std::istream&operator>>(std::istream&str,SyncAB_results&item){
  return str>>item.position>>item.width>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncAB_results&item){
  return str<<item.position<<"\t"<<item.width<<"\t"<<item.chi_sq;
}
struct SyncOposite_results{
  MathTemplates::value<double>position,width;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncOposite_results&item){
  return str>>item.position>>item.width>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncOposite_results&item){
  return str<<item.position<<"\t"<<item.width<<"\t"<<item.chi_sq;
}
struct SyncNeighbour_results{
  MathTemplates::value<double>position_left,width_left,position_right,width_right;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncNeighbour_results&item){
  return str>>item.position_left>>item.width_left>>item.position_right>>item.width_right>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncNeighbour_results&item){
  return str<<item.position_left<<"\t"<<item.width_left<<"\t"<<item.position_right<<"\t"<<item.width_right<<"\t"<<item.chi_sq;
}
#endif