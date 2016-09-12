#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
struct SyncAB_results{
  MathTemplates::value<double>position,width;double chi_sq;
  inline const MathTemplates::value<double> Range()const{return {position.val(),1.0*width.val()};}
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
struct SyncOposite_results{
  MathTemplates::value<double>position,width;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncOposite_results&item){
  double v,u;
  str>>v>>u;item.position={v,u};
  str>>v>>u;item.width={v,u};
  str>>item.chi_sq;
  return str;
}
inline std::ostream&operator<<(std::ostream&str,const SyncOposite_results&item){
  str<<item.position.val()<<"\t"<<item.position.uncertainty()<<"\t"<<item.width.val()<<"\t"<<item.width.uncertainty()<<"\t"<<item.chi_sq;
  return str;
}
struct SyncNeighbour_results{
  MathTemplates::value<double>position_left,width_left,position_right,width_right;double chi_sq;
};
inline std::istream&operator>>(std::istream&str,SyncNeighbour_results&item){
  double v,u;
  str>>v>>u;item.position_left={v,u};
  str>>v>>u;item.width_left={v,u};
  str>>v>>u;item.position_right={v,u};
  str>>v>>u;item.width_right={v,u};
  str>>item.chi_sq;
  return str;
}
inline std::ostream&operator<<(std::ostream&str,const SyncNeighbour_results&item){
  str
  <<item.position_left.val()<<"\t"<<item.position_left.uncertainty()<<"\t"<<item.width_left.val()<<"\t"<<item.width_left.uncertainty()
  <<item.position_right.val()<<"\t"<<item.position_right.uncertainty()<<"\t"<<item.width_right.val()<<"\t"<<item.width_right.uncertainty()
  <<"\t"<<item.chi_sq;
  return str;
}
#endif