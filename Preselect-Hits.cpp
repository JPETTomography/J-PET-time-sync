#include <iostream>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/PrepareHits.h>
using namespace std;
void AddModules() {
	auto map=make_JPetMap<TOT_cut>();
	cin>>(*map);
	JPetManager::getManager().registerTask([map](){
		return new JPetTaskLoader("Signals","Hits",new PrepareHits("TOF cut","",map));
	});
}
