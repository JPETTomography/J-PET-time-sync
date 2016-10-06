#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/SyncAB.h>
using namespace std;
void AddModules() {
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("phys.hit.means", "Stat4AB",new TaskSyncAB("Synchronization A-B",""));
	});
}
