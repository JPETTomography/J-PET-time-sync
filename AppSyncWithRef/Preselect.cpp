#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/TaskA.h>
#include <Preselection/TaskB.h>
#include <Preselection/TaskC.h>
#include <Preselection/TaskD.h>
#include <Preselection/SyncRef.h>
using namespace std;
void AddModules() {
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("hld", "tslot.raw", new TaskA("Module A: Unp to TSlot Raw","Process unpacked HLD file into a tree of JPetTSlot objects"));
	});
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("tslot.raw", "raw.sig",new TaskB("Module B: TSlot Cal to Raw Signal","Build Raw Signals from Calibrated TSlots"));
	});
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("raw.sig", "phys.hit",new TaskC("Module C: Pair signals","Create hits from pairs of signals")); 
	}); 
	JPetManager::getManager().registerTask([](){
	    return new JPetTaskLoader("phys.hit", "phys.hit.means",new TaskD("Module D: Make histograms for hits","Only make timeDiff histos and produce mean timeDiff value for each threshold and slot to be used by the next module"));
	});
	JPetManager::getManager().registerTask([](){
	    return new JPetTaskLoader("phys.hit.means", "ref.spectra",new TaskSyncRef("RefSync:",""));
	});
	
}
