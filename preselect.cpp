#include <DBHandler/HeaderFiles/DBHandler.h>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/timesyncpreselector.h>
using namespace std;
int main(int argc, char* argv[]) {
	DB::SERVICES::DBHandler::createDBConnection("../DBConfig/configDB.cfg");
	JPetManager& manager = JPetManager::getManager();
	manager.parseCmdLine(argc, argv);
	
	// Here create all analysis modules to be used:
	manager.registerTask([](){
		return new JPetTaskLoader("hld", "tslot.raw",
					  new TaskA("Module A: Unp to TSlot Raw",
						    "Process unpacked HLD file into a tree of JPetTSlot objects"));
	});    
	
	manager.run();
}
