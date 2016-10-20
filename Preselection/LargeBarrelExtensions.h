#ifndef _LARGE_BARREL_EXTENSIONS_
#define _LARGE_BARREL_EXTENSIONS_
#include <map>
#include <vector>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetHit/JPetHit.h>
class LargeBarrelMapping{
public:
  
  LargeBarrelMapping();
  LargeBarrelMapping(const JPetParamBank & paramBank);
  virtual ~LargeBarrelMapping();
  int getLayerNumber(const JPetLayer & layer) const;
  int getNumberOfSlots(const JPetLayer & layer) const;
  int getNumberOfSlots(int layerNumber) const;
  int getSlotNumber(const JPetBarrelSlot & slot) const;
  int calcDeltaID(const JPetBarrelSlot& slot1,const JPetBarrelSlot& slot2) const;
  int opositeDeltaID(const JPetLayer& layer) const;
  void buildMappings(const JPetParamBank & paramBank);
  const int calcGlobalPMTNumber(const JPetPM & pmt) const;
private:
  std::map<double, int> fRadiusToLayer;
  std::vector<std::map<double, int> > fThetaToSlot;
  std::vector<int> fNumberOfSlotsInLayer;
};

const double TimeOfSignal(const JPetPhysSignal&signal);
#endif
