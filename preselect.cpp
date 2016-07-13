#include <DBHandler/HeaderFiles/DBHandler.h>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/TaskA.h>
#include <Preselection/TaskB1.h>
using namespace std;
int main(int argc, char* argv[]) {
	DB::SERVICES::DBHandler::createDBConnection("configDB.cfg");
	JPetManager& manager = JPetManager::getManager();
	manager.parseCmdLine(argc, argv);
	manager.registerTask([](){
		return new JPetTaskLoader("hld", "tslot.raw", new TaskA("Module A: Unp to TSlot Raw","Process unpacked HLD file into a tree of JPetTSlot objects"));
	});
	manager.registerTask([](){
		return new JPetTaskLoader("tslot.raw", "raw.sig",new TaskB1("Module C1: TSlot Cal to Raw Signal","Build Raw Signals from Calibrated TSlots"));
	});
	manager.run();
}
