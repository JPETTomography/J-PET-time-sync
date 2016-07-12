#ifndef ______TIMESYNC_PRESELECTION_H________
#	define ______TIMESYNC_PRESELECTION_H________
#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <JPetTask/JPetTask.h>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetParamManager/JPetParamManager.h>
#include <JPetSigCh/JPetSigCh.h>
class JPetWriter;
class TH1F;
class FrameworkTimeSyncPreselect: public JPetTask{
public:
	FrameworkTimeSyncPreselect(const char * name, const char * description);
	virtual void exec()override;
	virtual void terminate()override;
	virtual void setParamManager(JPetParamManager* paramManager)override;
	const JPetParamBank& getParamBank();
	virtual void setWriter(JPetWriter* writer)override;
protected:
	JPetParamManager* fParamManager;
	JPetWriter* fWriter;
	std::shared_ptr<TH1F> fHistHitsCount;
};
#endif
