#include <DBHandler/HeaderFiles/DBHandler.h>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/TaskA.h>
#include <Preselection/TaskB.h>
#include <Preselection/TaskC.h>
#include <Preselection/TaskD.h>
#include <Preselection/TaskTimeSync.h>
using namespace std;
int main(int argc, char* argv[]) {
	DB::SERVICES::DBHandler::createDBConnection("configDB.cfg");
	JPetManager& manager = JPetManager::getManager();
	manager.parseCmdLine(argc, argv);
	manager.registerTask([](){
		return new JPetTaskLoader("hld", "tslot.raw", new TaskA("Module A: Unp to TSlot Raw","Process unpacked HLD file into a tree of JPetTSlot objects"));
	});
	manager.registerTask([](){
		return new JPetTaskLoader("tslot.raw", "raw.sig",new TaskB("Module B: TSlot Cal to Raw Signal","Build Raw Signals from Calibrated TSlots"));
	});
	manager.registerTask([](){
		return new JPetTaskLoader("raw.sig", "phys.hit",new TaskC("Module C: Pair signals","Create hits from pairs of signals")); 
	}); 
	manager.registerTask([](){
		return new JPetTaskLoader("phys.hit", "phys.hit.means",new TaskD("Module D: Make histograms for hits","Only make timeDiff histos and produce mean timeDiff value for each threshold and slot to be used by the next module"));
	});
	manager.registerTask([](){
		return new JPetTaskLoader("phys.hit.means", "phys.hit.means.timesyncstat",new TimeSyncTask("TimeSyncTask",""));
	});
	manager.run();
}
