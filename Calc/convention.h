// this file is distributed under 
// MIT license
#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
#include <JPetLargeBarrelExtensions/PetDict.h>


// Size of J-PET detector
template<class DataType>
inline const std::shared_ptr<JPetMap<DataType>> make_JPetMap(){
  return std::shared_ptr<JPetMap<DataType>>(new JPetMap<DataType>({48,48,96}));
}


//Thresholds
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


//Sync AB
struct SyncAB_results{
    MathTemplates::value<double>peak;double chi_sq;
    inline const bool valid()const{
      return (chi_sq>=0)&&(peak.uncertainty()<=1.0)&&(peak.uncertainty()>=0.1);
    }
};
inline std::istream&operator>>(std::istream&str,SyncAB_results&item){
  return str>>item.peak>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncAB_results&item){
  return str<<item.peak<<"\t"<<item.chi_sq;
}

//Hit-hit coincidences: oposite
struct SyncOposite_results{
    MathTemplates::value<double>peak;double chi_sq;
    inline const bool valid()const{
      return (chi_sq>=0)&&(peak.uncertainty()<=1.0)&&(peak.uncertainty()>=0.1);
    }
};
inline std::istream&operator>>(std::istream&str,SyncOposite_results&item){
  return str>>item.peak>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncOposite_results&item){
  return str<<item.peak<<"\t"<<item.chi_sq;
}
inline const std::shared_ptr<JPetMap<SyncOposite_results>> make_OpoCoiMap(){
  return std::shared_ptr<JPetMap<SyncOposite_results>>(new JPetMap<SyncOposite_results>({24,24,48}));
}



//Hit-hit coincidences for neighbour strips in layer
struct SyncScatter_results{
  MathTemplates::value<double>left,right,assymetry;double chi_sq;
    inline const bool valid()const{
    return (chi_sq>=0)&&
	(assymetry.val()<=5)&&(assymetry.val()>=0.2)&&
	(left.uncertainty()<=2.0)&&(left.uncertainty()>=0.2)&&
	(right.uncertainty()<=2.0)&&(right.uncertainty()>=0.2)&&
	((left.uncertainty()/right.uncertainty())<=1.4)&&
	((right.uncertainty()/left.uncertainty())<=1.4)&&
	(right.val()-left.val())>1.0;
    }
};
inline std::istream&operator>>(std::istream&str,SyncScatter_results&item){
  return str>>item.left>>item.right>>item.assymetry>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncScatter_results&item){
  return str<<item.left<<"\t"<<item.right<<"\t"<<item.assymetry<<"\t"<<item.chi_sq;
}
const std::vector<size_t> neighbour_delta_id{1,2};


//hit-hit inter-layer coincidences
struct SyncLayerIndex{size_t coef,offs;};
const std::vector<std::vector<SyncLayerIndex>> SyncLayerIndices{
    {{.coef=1,.offs=1},{.coef=1,.offs=48-2}},
    {{.coef=2,.offs=4},{.coef=2,.offs=96-4}}
};
struct SyncLayer{SyncScatter_results zero,one;};
inline std::istream&operator>>(std::istream&str,SyncLayer&item){
  return str>>item.zero>>item.one;
}
inline std::ostream&operator<<(std::ostream&str,const SyncLayer&item){
  return str<<item.zero<<"\n"<<item.one;
}
inline const std::shared_ptr<JPetMap<SyncLayer>> make_InterLayerMap(){
  return std::shared_ptr<JPetMap<SyncLayer>>(new JPetMap<SyncLayer>({48,48}));
}
#endif
