// this file is distributed under 
// MIT license
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/SyncAB.h>
void AddModules() {
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("Hits", "Stat4AB",new TaskSyncAB("Synchronization A-B",""));
	});
}
