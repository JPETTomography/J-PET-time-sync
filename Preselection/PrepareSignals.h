#ifndef TASKA_H
#define TASKA_H

#include <vector>
#include <memory>
#include <JPetTask/JPetTask.h>
#include <JPetTimeWindow/JPetTimeWindow.h>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetParamManager/JPetParamManager.h>
class LargeBarrelMapping;
class JPetWriter;
class PrepareSignals: public JPetTask{
public:
  PrepareSignals(const char * name, const char * description);
  virtual ~PrepareSignals();
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
  virtual void setWriter(JPetWriter* writer)override;
  virtual void setParamManager(JPetParamManager* paramManager)override;
  const JPetParamBank& getParamBank()const;
private:
  JPetWriter* fWriter;
  JPetParamManager* fParamManager;
  long long int fCurrEventNumber;
  std::shared_ptr<LargeBarrelMapping> fBarrelMap;
};
#endif
