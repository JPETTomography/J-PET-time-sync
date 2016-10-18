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
    if(argc<2){
	cerr<<"Usage: "<<argv[0]<<" <filename> <filename> ..."<<endl;
	return -1;
    }
    vector<string> root_filenames;
    for(int i=1;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    Plotter::Instance().SetOutput(".","test");
    auto map=make_JPetMap<char>();
    for(size_t layer=1;layer<=map->LayersCount();layer++){
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
	    const auto name=LayerSlotThr(layer,slot,1);
	    const auto shist=ReadHist(root_filenames,name);
	    Plot<double>().Hist(shist,name)<<"set key on"<<"set xrange [-20:20]";
	    if((layer==1)&&(slot<5))
		cout<<layer<<"\t"<<slot<<"\t"<<Sync::Fit4SyncAB(shist,"SyncAB "+name,1).peak<<"\n";
	    
	}
    }
    return 0;
}
