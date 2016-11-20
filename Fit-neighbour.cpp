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
    size_t thr_cnt=0;
    {
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
    vector<shared_ptr<JPetMap<SyncNeighbour_results>>> Nei;
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
	Nei.push_back(make_JPetMap<SyncNeighbour_results>());
    Plotter::Instance().SetOutput(".","strips-neighbour");
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
    for(size_t layer=1;layer <= Nei[i]->LayersCount();layer++){
	hist<double> left,right,assym;
	SortedPoints<double> chisq;
	for(size_t slot=1;slot<=Nei[i]->LayerSize(layer);slot++){
	    const auto name=LayerSlotThr(layer,slot,1)+"-deltaid"+to_string(neighbour_delta_id[i]);
	    const auto shist=ReadHist(root_filenames,"DeltaT-with-neighbour-"+name);
	    auto& item=Nei[i]->item({.layer=layer,.slot=slot});
	    if(shist.TotalSum().val()>=5.){
		item=Sync::Fit4SyncNeighbour(shist,"Neighbour "+name,thr_cnt);
		left<<point<value<double>>(double(slot),item.left);
		right<<point<value<double>>(double(slot),item.right);
		assym<<point<value<double>>(double(slot),item.assymetry);
		chisq<<point<double>(double(slot),item.chi_sq);
	    }else{
		Plot<double>().Hist(shist);
		item={.left=0,.right=0,.assymetry=0,.chi_sq=-1};
	    }
	}
	const string title="set title 'deltaID="+to_string(neighbour_delta_id[i])+"'";
	Plot<double>().Hist(left,"Position Left").Hist(right,"Position Right")<<"set key on"<<title;
	Plot<double>().Hist(assym,"Assymetry of peaks height")<<"set key on"<<"set yrange [0:]"<<title;
	Plot<double>().Line(chisq,"Chi^2")<<"set key on"<<"set yrange [0:]"<<title;
    }
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)cout<<(*Nei[i]);
    return 0;
}
