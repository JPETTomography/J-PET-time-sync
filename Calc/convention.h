// this file is distributed under 
// MIT license
#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <iostream>
#include <math_h/sigma.h>
#include <JPetLargeBarrelExtensions/PetDict.h>

#include <j-pet-config.h>

// Size of J-PET detector
template<class DataType>
inline const std::shared_ptr<JPetMap<DataType>> make_JPetMap(){
  return std::shared_ptr<JPetMap<DataType>>(new JPetMap<DataType>(Full_J_PET_SIZE));
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
    MathTemplates::value<double>peak;double chi_sq,uncertainty_estimation;
    inline const bool valid()const{
      return (chi_sq>=0)&&
      (peak.uncertainty()<=1.0*TIME_UNIT_CONST)&&
      (peak.uncertainty()>=0.1*TIME_UNIT_CONST);
    }
};
inline std::istream&operator>>(std::istream&str,SyncAB_results&item){
  return str>>item.peak>>item.chi_sq>>item.uncertainty_estimation;
}
inline std::ostream&operator<<(std::ostream&str,const SyncAB_results&item){
  return str<<item.peak<<"\t"<<item.chi_sq<<"\t"
  <<item.uncertainty_estimation;
}

//Hit-hit coincidences: oposite
struct SyncOposite_results{
    MathTemplates::value<double>peak;double chi_sq;
    inline const bool valid()const{
      return (chi_sq>=0)&&
      (peak.uncertainty()<=1.5*TIME_UNIT_CONST)&&
      (peak.uncertainty()>=0.1*TIME_UNIT_CONST);
    }
};
inline std::istream&operator>>(std::istream&str,SyncOposite_results&item){
  return str>>item.peak>>item.chi_sq;
}
inline std::ostream&operator<<(std::ostream&str,const SyncOposite_results&item){
  return str<<item.peak<<"\t"<<item.chi_sq;
}
inline const std::shared_ptr<JPetMap<SyncOposite_results>> make_OpoCoiMap(){
  return std::shared_ptr<JPetMap<SyncOposite_results>>(new JPetMap<SyncOposite_results>(Half_J_PET_SIZE));
}



//Hit-hit coincidences for neighbour strips in layer
struct SyncScatter_results{
    MathTemplates::value<double>left,right,assymetry;double chi_sq;
    inline const bool valid()const{
    return (chi_sq>=0)&&
	(assymetry.val()<=15.0)&&(assymetry.val()>=0.6667)&&
	(left.uncertainty()<=3.0*TIME_UNIT_CONST)&&
	(left.uncertainty()>=0.1*TIME_UNIT_CONST)&&
	(right.uncertainty()<=3.0*TIME_UNIT_CONST)&&
	(right.uncertainty()>=0.1*TIME_UNIT_CONST)&&
	((left.uncertainty()/right.uncertainty())<=6.0)&&
	((right.uncertainty()/left.uncertainty())<=6.0)&&
	(right.val()-left.val())>1.0*TIME_UNIT_CONST;
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
const std::vector<std::vector<SyncLayerIndex>> SyncLayerIndices{InterLayer_Indices};
struct SyncLayer{SyncScatter_results zero,one;};
inline std::istream&operator>>(std::istream&str,SyncLayer&item){
  return str>>item.zero>>item.one;
}
inline std::ostream&operator<<(std::ostream&str,const SyncLayer&item){
  return str<<item.zero<<"\n"<<item.one;
}
inline const std::shared_ptr<JPetMap<SyncLayer>> make_InterLayerMap(){
  return std::shared_ptr<JPetMap<SyncLayer>>(
      new JPetMap<SyncLayer>(InterLayer_J_PET_SIZE)
  );
}
#endif
