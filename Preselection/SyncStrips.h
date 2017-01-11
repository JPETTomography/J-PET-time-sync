// this file is distributed under 
// MIT license
#ifndef _____TIME___SYNC____TASK_________ 
#       define _____TIME___SYNC____TASK_________
#include <memory>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <JPetLargeBarrelExtensions/BarrelExtensions.h>
class SyncAB_results;
class Synchronization;
class TaskSyncStrips:public TOT_Hists{
public:
  TaskSyncStrips(const char * name, const char * description);
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
private:
  void fillCoincidenceHistos();
  std::vector<JPetHit> fHits;
  std::shared_ptr<JPetMap<SyncAB_results>> f_AB_position;
  std::shared_ptr<Synchronization> fSync;
};
#endif
