#ifndef TASKC_H 
#define TASKC_H 

#include <JPetTask/JPetTask.h>
#include <JPetHit/JPetHit.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <Calc/convention.h>
class LargeBarrelMapping;
class JPetWriter;
class PrepareHits:public JPetTask {
public:
  PrepareHits(const char * name, const char * description);
  virtual ~PrepareHits();
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
  virtual void setWriter(JPetWriter* writer)override;
private:
  void createAndStoreHits();
  std::vector<JPetRawSignal> fSignals;
  JPetWriter* fWriter;
  std::shared_ptr<LargeBarrelMapping>fBarrelMap;
  std::shared_ptr<JPetMap<TOT_cut>>f_map;
};
#endif
