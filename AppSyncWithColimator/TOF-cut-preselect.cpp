#include <iostream>
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include <Preselection/PreselectionMain.h>
#include <Preselection/TOFCut.h>
using namespace std;
void AddModules() {
	auto map=make_JPetMap<TOT_cut>();
	cin>>(*map);
	JPetManager::getManager().registerTask([map](){
		return new JPetTaskLoader("phys.hit","phys.hit.TOF.cut",new TOFCut("TOF cut","",map));
	});
}
