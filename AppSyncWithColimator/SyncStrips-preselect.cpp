#include <iostream>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/SyncStrips.h>
using namespace std;
void AddModules() {
    auto map=make_JPetMap<SyncAB_results>();
    cin>>(*map);
    JPetManager::getManager().registerTask([map](){
	return new JPetTaskLoader("phys.hit.means", "Stat4Strips",new TaskSyncStrips(map,"Synchronization of strips",""));
    });
}
