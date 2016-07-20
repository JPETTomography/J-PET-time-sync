#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/SyncStrips.h>
using namespace std;
void AddModules() {
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("phys.hit.means", "Stat4Strips",new TaskSyncStrips("Synchronization of strips",""));
	});
}
