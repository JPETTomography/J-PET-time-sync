#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/PrepareHits.h>
void AddModules() {
	JPetManager::getManager().registerTask([](){
		return new JPetTaskLoader("Signals","Hits",new PrepareHits("TOF cut",""));
	});
}
