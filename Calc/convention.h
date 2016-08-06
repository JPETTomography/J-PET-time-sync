#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
struct SyncAB_results{
  MathTemplates::value<double>position,width;double chi_sq;
  inline const MathTemplates::value<double> Range()const{return {position.val(),width.val()};}
};
inline std::istream&operator>>(std::istream&str,SyncAB_results&item){
  double v,u;
  str>>v>>u;item.position={v,u};
  str>>v>>u;item.width={v,u};
  str>>item.chi_sq;
  return str;
}
inline std::ostream&operator<<(std::ostream&str,const SyncAB_results&item){
  str<<item.position.val()<<"\t"<<item.position.uncertainty()<<"\t"<<item.width.val()<<"\t"<<item.width.uncertainty()<<"\t"<<item.chi_sq;
  return str;
}
#endif