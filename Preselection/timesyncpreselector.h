#ifndef ______TIMESYNC_PRESELECTION_H________
#	define ______TIMESYNC_PRESELECTION_H________
#include <vector>
#include <functional>
#include <utility>
#include <JPetTask/JPetTask.h>
#include <JPetParamBank/JPetParamBank.h>
#include <JPetParamManager/JPetParamManager.h>
#include <JPetSigCh/JPetSigCh.h>
typedef std::function<void(const std::vector<std::pair<JPetSigCh,JPetSigCh>>&)> TDCFUNC;
class FrameworkTDCWrapper: public JPetTask{
public:
	FrameworkTDCWrapper(const char * name, const char * description, const TDCFUNC func);
	virtual void exec()override;
	virtual void terminate()override;
	virtual void setParamManager(JPetParamManager* paramManager)override;
	const JPetParamBank& getParamBank();
protected:
	JPetParamManager* fParamManager;
	TDCFUNC fFunction;
};
#endif
