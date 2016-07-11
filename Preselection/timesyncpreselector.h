#ifndef ______TIMESYNC_PRESELECTION_H________
#	define ______TIMESYNC_PRESELECTION_H________

#include <vector>
#include "JPetTask/JPetTask.h"
#include "JPetTimeWindow/JPetTimeWindow.h"
#include "JPetParamBank/JPetParamBank.h"
#include "JPetParamManager/JPetParamManager.h"
class JPetWriter;

class TaskA: public JPetTask{
public:
	TaskA(const char * name, const char * description);
	virtual void exec();
	virtual void terminate();
	virtual void setWriter(JPetWriter* writer) {
		fWriter = writer;
	}
	void setParamManager(JPetParamManager* paramManager) {
		fParamManager = paramManager;
	}
	const JPetParamBank& getParamBank() {
		assert(fParamManager);
		return fParamManager->getParamBank();
	}
protected:
	void saveTimeWindow( JPetTimeWindow slot);
	
	JPetWriter* fWriter;
	JPetParamManager* fParamManager;
	int fCurrEventNumber;
	
};
#endif
