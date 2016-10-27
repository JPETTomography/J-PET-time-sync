#ifndef _____TIME___SYNC____TASK_________ 
#       define _____TIME___SYNC____TASK_________
#include <memory>
#include <JPetTask/JPetTask.h>
#include <JPetHit/JPetHit.h>
#include <JPetRawSignal/JPetRawSignal.h>
class LargeBarrelMapping;
class JPetWriter;
class Synchronization;
class TaskSyncAB:public JPetTask {
public:
  TaskSyncAB(const char * name, const char * description);
  virtual ~TaskSyncAB();
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
  virtual void setWriter(JPetWriter* writer)override;
private:
  void fillCoincidenceHistos();
  std::vector<JPetHit>fHits;
  std::shared_ptr<LargeBarrelMapping>fBarrelMap;
  std::shared_ptr<Synchronization>fSync;
  JPetWriter*fWriter;
};
#endif
