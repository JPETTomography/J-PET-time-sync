#ifndef _LARGE_BARREL_EXTENSIONS_
#define _LARGE_BARREL_EXTENSIONS_
#include <map>
#include <vector>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetHit/JPetHit.h>
#include "PetDict.h"
class AbstractBarrelMapping{
public:
  virtual const size_t getLayersCount()const =0;
  virtual const size_t getLayerNumber(const JPetLayer & layer)const =0;
  virtual const size_t getSlotsCount(const JPetLayer & layer)const =0;
  virtual const size_t getSlotsCount(const size_t layer)const =0;
  virtual const size_t getSlotNumber(const JPetBarrelSlot & slot) const=0;
  const StripPos getStripPos(const JPetBarrelSlot & slot) const;
  const std::vector<size_t> getLayersSizes()const;
};
class LargeBarrelMapping:public AbstractBarrelMapping{
public:  
  LargeBarrelMapping(const JPetParamBank & paramBank);
  virtual ~LargeBarrelMapping();
  
  virtual const size_t getLayersCount()const override;
  virtual const size_t getLayerNumber(const JPetLayer & layer)const override;
  virtual const size_t getSlotsCount(const JPetLayer & layer)const override;
  virtual const size_t getSlotsCount(const size_t layer)const override;
  virtual const size_t getSlotNumber(const JPetBarrelSlot & slot) const override;
  const size_t calcDeltaID(const JPetBarrelSlot& slot1,const JPetBarrelSlot& slot2) const;
  const size_t calcGlobalPMTNumber(const JPetPM & pmt) const;
private:
  std::vector<double> fRadii;
  std::vector<std::vector<double>> fTheta;
};

const double TimeOfSignal(const JPetPhysSignal&signal);
#endif
