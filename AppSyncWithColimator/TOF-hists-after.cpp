#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <IO/PetDict.h>
#include <Calc/convention.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
    if(argc<3){
	cerr<<"Usage: "<<argv[0]<<" <filename> <filename> ..."<<endl;
	return -1;
    }
    vector<string> root_filenames;
    for(int i=1;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    Plotter::Instance().SetOutput(".","TOF-hists-after-cut");
    auto map=make_JPetMap<char>();
    for(size_t layer=1;layer<=map->LayersCount();layer++){
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
	    {
		const string name="TOT_"+LayerSlotThr(layer,slot,1)+"_A";
		const auto shist=ReadHist(root_filenames,name);
		Plot<double>().Hist(shist,name)<<"set key on";
	    }
	    {
		const string name="TOT_"+LayerSlotThr(layer,slot,1)+"_B";
		const auto shist=ReadHist(root_filenames,name);
		Plot<double>().Hist(shist,name)<<"set key on";
	    }
	}
    }
    return 0;
}
