#ifndef _____TIME___SYNC____TASK_________ 
#       define _____TIME___SYNC____TASK_________
#include <memory>
#include <JPetTask/JPetTask.h>
#include <JPetHit/JPetHit.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <j-pet-framework-extension/PetDict.h>
class LargeBarrelMapping;
class SyncAB_results;
class JPetWriter;
class Synchronization;
class TaskSyncStrips:public JPetTask {
public:
  TaskSyncStrips(const char * name, const char * description);
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
  virtual void setWriter(JPetWriter* writer)override;
private:
  JPetWriter* fWriter;
  void fillCoincidenceHistos();
  std::vector<JPetHit> fHits;
  std::shared_ptr<LargeBarrelMapping> fBarrelMap;
  std::shared_ptr<JPetMap<SyncAB_results>> f_AB_position;
  std::shared_ptr<Synchronization> fSync;
};
#endif
