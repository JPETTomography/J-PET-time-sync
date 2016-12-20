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
	cerr<<"Usage: "<<argv[0]<<" <filename> <filename> ..."<<endl;
	return -1;
    }
    vector<string> root_filenames;
    for(int i=1;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    vector<shared_ptr<JPetMap<SyncScatter_results>>> Nei;
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
	Nei.push_back(make_JPetMap<SyncScatter_results>());
    auto IL=make_InterLayerMap();
    Plotter::Instance().SetOutput(".","Scattered");
    //Neighbour strips
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
    for(size_t layer=1;layer <= Nei[i]->layersCount();layer++){
	hist<double> left,right,assym,chisq;
	for(size_t slot=1;slot<=Nei[i]->layerSize(layer);slot++){
	    const auto name=LayerSlot(layer,slot)+"-deltaid"+to_string(neighbour_delta_id[i]);
	    const auto shist=ReadHist(root_filenames,"DeltaT-with-neighbour-"+name);
	    auto& item=Nei[i]->item({.layer=layer,.slot=slot});
	    item=Sync::Fit4SyncScatter(shist,"Neighbour "+name,1);
	    if(item.valid()){
		left<<point<value<double>>(double(slot),item.left);
		right<<point<value<double>>(double(slot),item.right);
		assym<<point<value<double>>(double(slot),item.assymetry);
		chisq<<point<value<double>>(double(slot),item.chi_sq);
	    }
	}
	const string title="set title 'L="+to_string(layer)+";deltaID="+to_string(neighbour_delta_id[i])+"'";
	Plot<double>().Hist(left,"Position Left").Hist(right,"Position Right")<<"set key on"<<title;
	Plot<double>().Hist(assym,"Assymetry of peaks height")<<"set key on"<<"set yrange [0:]"<<title;
	Plot<double>().Hist(chisq,"Chi^2")<<"set key on"<<"set yrange [0:]"<<title;
    }
    //Inter-layer
    for(size_t layer=1;layer <= IL->layersCount();layer++)for(size_t i=0;i<2;i++){
	hist<double> left,right,assym,chisq;
	for(size_t slot=1;slot<=IL->layerSize(layer);slot++){
	    const auto name="Inter-layer-"+LayerSlot(layer,slot)+"-"+to_string(i);
	    const auto shist=ReadHist(root_filenames,name);
	    SyncScatter_results res=Sync::Fit4SyncScatter(shist,"IL "+name,1);
	    if(res.valid()){
		left<<point<value<double>>(double(slot),res.left);
		right<<point<value<double>>(double(slot),res.right);
		assym<<point<value<double>>(double(slot),res.assymetry);
		chisq<<point<value<double>>(double(slot),res.chi_sq);
	    }
	    switch(i){
		case 0: 
		    IL->item({.layer=layer,.slot=slot}).zero=res;
		    break;
		case 1: 
		    IL->item({.layer=layer,.slot=slot}).one=res;
		    break;
	    }
	}
	const string title="set title 'L="+to_string(layer)+";i="+to_string(i)+"'";
	Plot<double>().Hist(left,"Position Left").Hist(right,"Position Right")<<"set key on"<<title;
	Plot<double>().Hist(assym,"Assymetry of peaks height")<<"set key on"<<"set yrange [0:]"<<title;
	Plot<double>().Hist(chisq,"Chi^2")<<"set key on"<<"set yrange [0:]"<<title;
    }
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)cout<<(*Nei[i]);
    cout<<(*IL);
    return 0;
}
