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
	if(argc=1){
		char * frameworkargs[2];
		frameworkargs[0]=argv[0];
		frameworkargs[1]="--help";
		cout<<"AUTOMATICALLY GIVEN OPTIONS -l and -i, don't give them once more"<<endl;
		JPetManager::getManager().parseCmdLine(2, frameworkargs);
	}else{
		char * frameworkargs[argc+4];
		frameworkargs[0]=argv[0];
		frameworkargs[1]="-l";
		frameworkargs[2]="large_barrel.json";
		frameworkargs[3]="-i";
		frameworkargs[4]="43";
		for(size_t i=1;i<argc;i++)frameworkargs[i+4]=argv[i];
		JPetManager::getManager().parseCmdLine(argc+4, frameworkargs);
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
			return new JPetTaskLoader("phys.hit.means", "phys.hit.means.timesyncstat",new TimeSyncTask("TimeSyncTask",""));
		});
		JPetManager::getManager().run();
	}
}
