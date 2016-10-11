#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/PrepareSignals.h>
using namespace std;
void AddModules() {
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("hld", "Signals", new PrepareSignals("Preparing signals",""));
	});
}
