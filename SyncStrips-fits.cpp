#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <IO/PetDict.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
    if(argc<3){
	cerr<<"Usage: "<<argv[0]<<" <thread_count> <filename> <filename> ..."<<endl;
	return -1;
    }
    auto map=make_JPetMap<int>();
    
    string parameters_file(argv[1]);
    vector<string> root_filenames;
    for(int i=2;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    Plotter::Instance().SetOutput(".","strips-synchro");
    for(size_t layer=1;layer<=map->LayersCount();layer++){
	for(size_t slot=1;slot<=(map->LayerSize(layer)/2);slot++)
	    Plot<double>().Hist(ReadHist(root_filenames,"Delta_t_with_oposite_"+LayerSlotThr(layer,slot,1)),"Oposite "+LayerSlotThr(layer,slot,1))<<"set key on"<<"set xrange [-30:30]";
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++)
	    Plot<double>().Hist(ReadHist(root_filenames,"Delta_t_with_neighbour_"+LayerSlotThr(layer,slot,1)+"_deltaid3"),"Neighbour "+LayerSlotThr(layer,slot,1)+" deltaid=3")<<"set key on"<<"set xrange [-30:30]";
    }
    
    return 0;
}
