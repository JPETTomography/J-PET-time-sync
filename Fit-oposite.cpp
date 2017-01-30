// this file is distributed under 
// MIT license
#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <JPetLargeBarrelExtensions/BarrelExtensions.h>
#include <Calc/SyncProcedures.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
    if(argc<2){
	cerr<<"Usage: "<<argv[0]
	<<" <thread_count> <filename> <filename> ..."<<endl;
	return -1;
    }
    vector<string> root_filenames;
    for(int i=1;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    auto map=make_OpoCoiMap();
    Plotter::Instance().SetOutput(".","Oposite");
    for(size_t layer=1;layer <= map->layersCount();layer++){
	hist<double> position,chisq;
	for(size_t slot=1;slot<=map->layerSize(layer);slot++){
	    const auto name=LayerSlot(layer,slot);
	    const auto shist=ReadHist(
		root_filenames,"DeltaT-with-oposite-"+name
	    );
	    auto& item=map->item({.layer=layer,.slot=slot});
	    item=Sync::Fit4SyncOposite(shist,"Oposite "+name,1);
	    if(item.valid()){
		position<<point<value<double>>(double(slot),item.peak);
		chisq<<point<value<double>>(double(slot),item.chi_sq);
	    }
	}
	Plot<double>().Hist(position,"Position")
	<<"set key on"<<"set title 'Layer "+to_string(layer)+"'";
	Plot<double>().Hist(chisq,"Chi^2")
	<<"set key on"<<"set yrange [0:]"
	<<"set title 'Layer "+to_string(layer)+"'";
    }
    cout<<(*map);
    return 0;
}
