#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <j-pet-framework-extension/PetDict.h>
#include <j-pet-framework-extension/BarrelExtensions.h>
#include <Calc/SyncProcedures.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
	if(argc<3){
		cerr<<"Usage: "<<argv[0]<<" <thread_count> <filename> <filename> ..."<<endl;
		return -1;
	}
	size_t thr_cnt=0;{
		stringstream thr_count(argv[1]);
		thr_count>>thr_cnt;
		if(0==thr_cnt){
			cerr<<"Wrong threads count"<<endl;
			cerr<<"Usage: "<<argv[0]<<" <thread_count> <filename> <filename> ..."<<endl;
			return -1;
		}
	}
	vector<string> root_filenames;
	for(int i=2;i<argc;i++)
		root_filenames.push_back(string(argv[i]));
	Plotter::Instance().SetOutput(".","AB-synchro");
	auto map=make_JPetMap<SyncAB_results>();
	for(size_t layer=1;layer<=map->LayersCount();layer++){
		hist<double> position;
		SortedPoints<double> chisq;
		for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
			const auto name=LayerSlot(layer,slot);
			const auto shist=ReadHist(root_filenames,name);
			auto&item=map->item({.layer=layer,.slot=slot});
			item=Sync::Fit4SyncAB(shist,"SyncAB "+name,thr_cnt);
			position<<point<value<double>>(double(slot),item.peak);
			chisq<<point<double>(double(slot),item.chi_sq);
		}
		Plot<double>().Hist(position,"Position")<<"set key on"<<"set title 'Layer "+to_string(layer)+"'";
		Plot<double>().Line(chisq,"Chi^2")<<"set key on"<<"set title 'Layer "+to_string(layer)+"'";
	}
	cout<<(*map);
	return 0;
}
