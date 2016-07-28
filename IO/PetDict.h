#ifndef ______PET__DICT___H_________
#       define ______PET__DICT___H_________
#include <list>
#include <vector>
#include <functional>
#include <math_h/error.h>
template<class DataType>
class JPetMap{
private:
  std::vector<std::vector<DataType>> f_data;
public:
  JPetMap(const std::initializer_list<size_t>&layer_sizes,const std::function<const DataType(const size_t layer,const size_t slot)>F){
    size_t layer_index=0;
    for(const size_t&item:layer_sizes){
      f_data.push_back(std::vector<DataType>);
      for(size_t slot=1;slot<=item;slot++)
        f_data[layer_index].push_back(F(layer_index+1,slot));
      layer_index++;
    }
  }
  virtual ~JPetMap(){}
  const size_t LayersCount()const{
    return f_data.size();
  }
  const size_t LayerSize(const size_t layer)const{
    if(layer==0)throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(layer>f_data.size())throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    return f_data[layer-1].size();
  }
  const DataType&operator()(const size_t layer, const size_t slot)const{
    if(layer==0)throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(layer>f_data.size())throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(slot==0)throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    if(slot>f_data[layer-1].size())throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    return f_data[layer-1][slot-1];
  }
  DataType&Item(const size_t layer, const size_t slot){
    if(layer==0)throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(layer>f_data.size())throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(slot==0)throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    if(slot>f_data[layer-1].size())throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    return f_data[layer-1][slot-1];
  }
};
#endif