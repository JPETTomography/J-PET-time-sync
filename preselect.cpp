#include <DBHandler/HeaderFiles/DBHandler.h>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/timesyncpreselector.h>
using namespace std;
int main(int argc, char* argv[]) {
	DB::SERVICES::DBHandler::createDBConnection("configDB.cfg");
	JPetManager& manager = JPetManager::getManager();
	manager.parseCmdLine(argc, argv);
	manager.registerTask([](){return new JPetTaskLoader("hld", "tslot.raw",new FrameworkTimeSyncPreselect("",""));});
	manager.run();
}
