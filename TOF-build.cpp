#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/TOFStat.h>
using namespace std;
void AddModules() {
    JPetManager::getManager().registerTask([](){
	return new JPetTaskLoader("phys.hit", "TOF.stat",new TOFStat("TOF stat: create TOF histograms","prepairs TOF statistics for making TOF cut"));
    });
}
