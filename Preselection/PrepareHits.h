#ifndef TASKC_H 
#define TASKC_H 

#include <JPetTask/JPetTask.h>
#include <JPetHit/JPetHit.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <Calc/convention.h>
#include <LargeBarrelExtensions/BarrelExtensions.h>
class PrepareHits:public TOT_Hists {
public:
  PrepareHits(const char * name, const char * description);
  virtual ~PrepareHits();
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
private:
  void createAndStoreHits();
  std::vector<JPetRawSignal> fSignals;
  long long int fCurrEventNumber;
};
#endif
