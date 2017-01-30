// this file is distributed under 
// MIT license
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/SyncStrips.h>
void AddModules() {
    JPetManager::getManager().registerTask([](){
	return new JPetTaskLoader(
	    "Hits", "Stat4Strips",new TaskSyncStrips(
		"Synchronization of strips",""
	    )
	);
    });
}
