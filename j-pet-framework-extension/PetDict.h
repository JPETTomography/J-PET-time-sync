#ifndef ______PET__DICT___H_________
#       define ______PET__DICT___H_________
#include <vector>
#include <functional>
#include <math_h/error.h>
struct StripPos{size_t layer,slot;};
template<class DataType>
class JPetMap{
private:
  std::vector<std::vector<DataType>> f_data;
public:
  JPetMap(const std::vector<size_t>&layer_sizes){
    size_t layer_index=0;
    for(const size_t&item:layer_sizes){
      f_data.push_back(std::vector<DataType>());
      for(size_t slot=1;slot<=item;slot++){
        DataType v;
        f_data[layer_index].push_back(v);
      }
      layer_index++;
    }
  }
  virtual ~JPetMap(){}
  void Read(std::istream&str){
    for(auto&layer:f_data)
      for(auto&item:layer)
        str>>item;
  }
  void Save(std::ostream&str)const{
    for(const auto&layer:f_data)
      for(const auto&item:layer)
        str<<item<<"\n";
  }
  const size_t LayersCount()const{
    return f_data.size();
  }
  const size_t LayerSize(const size_t layer)const{
    if(layer==0)throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(layer>f_data.size())throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    return f_data[layer-1].size();
  }
  const DataType&item(const StripPos&pos)const{
    if(pos.layer==0)throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(pos.layer>f_data.size())throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(pos.slot==0)throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    if(pos.slot>f_data[pos.layer-1].size())throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    return f_data[pos.layer-1][pos.slot-1];
  }
  const DataType&operator[](const StripPos&pos)const{return item(pos);}
  DataType&var_item(const StripPos&pos){
    if(pos.layer==0)throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(pos.layer>f_data.size())throw MathTemplates::Exception<JPetMap>("Invalid layer index");
    if(pos.slot==0)throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    if(pos.slot>f_data[pos.layer-1].size())throw MathTemplates::Exception<JPetMap>("Invalid slot index");
    return f_data[pos.layer-1][pos.slot-1];
  }
};
template<class DataType>
inline std::istream&operator>>(std::istream&str,JPetMap<DataType>&item){
  item.Read(str);
  return str;
}
template<class DataType>
inline std::ostream&operator<<(std::ostream&str,const JPetMap<DataType>&item){
  item.Save(str);
  return str;
}
#endif