#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <IO/PetDict.h>
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
    auto map=make_half_JPetMap<SyncOposite_results>();
    Plotter::Instance().SetOutput(".","strips-synchro");
    for(size_t layer=1;layer<=map->LayersCount();layer++)
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++)
	    map->Item(layer,slot)=Sync::Fit4SyncOposite(ReadHist(root_filenames,"Delta_t_with_oposite_"+LayerSlotThr(layer,slot,1)),"Oposite",thr_cnt);
    cout<<(*map);
    return 0;
}
