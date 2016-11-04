#ifndef _LARGE_BARREL_EXTENSIONS_
#define _LARGE_BARREL_EXTENSIONS_
#include <map>
#include <vector>
#include <string>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetHit/JPetHit.h>
#include <JPetTask/JPetTask.h>
#include "PetDict.h"
const std::string LayerThr(const size_t layer, const size_t thr);
const std::string LayerSlotThr(const size_t layer,const size_t slot, const size_t thr);
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
class LargeBarrelTask:public JPetTask{
public:
    virtual void init(const JPetTaskInterface::Options& opts)override;
    virtual void setWriter(JPetWriter* writer)override;
protected:
    JPetWriter&writter()const;
    const LargeBarrelMapping&map()const;
private:
    std::shared_ptr<LargeBarrelMapping>fBarrelMap;
    JPetWriter*fWriter;
};
class TOT_Hists:public LargeBarrelTask{
public:
    virtual void init(const JPetTaskInterface::Options& opts)override;
protected:
    void createHistos(const std::string&suffix);
    void fillHistos(const JPetHit&hit,const std::string&suffix);
};


#endif
