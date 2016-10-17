#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/TestAB.h>
using namespace std;
void AddModules() {
    JPetManager::getManager().registerTask([](){
	return new JPetTaskLoader("Hits", "TestStat",new TestSyncAB("Test",""));
    });
}
