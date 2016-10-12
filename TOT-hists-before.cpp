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
    Plotter::Instance().SetOutput(".","TOF-hists");
    auto map=make_JPetMap<TOT_cut>();
    for(size_t layer=1;layer<=map->LayersCount();layer++){
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
	    {
		const string name="TOT_"+LayerSlotThr(layer,slot,1)+"_A";
		const auto shist=ReadHist(root_filenames,name);
		Plot<double>().Hist(shist,name)<<"set key on"<<"set xrange [30:60]";
		double numerator=0,denominator=0;
		for(const auto&point:shist){
		    numerator+=point.Y().val()*point.X().val();
		    denominator+=point.Y().val();
		}
		map->Item(layer,slot).A=numerator/denominator;
	    }
	    {
		const string name="TOT_"+LayerSlotThr(layer,slot,1)+"_B";
		const auto shist=ReadHist(root_filenames,name);
		Plot<double>().Hist(shist,name)<<"set key on"<<"set xrange [30:60]";
		double numerator=0,denominator=0;
		for(const auto&point:shist){
		    numerator+=point.X().val()*point.Y().val();
		    denominator+=point.Y().val();
		}
		map->Item(layer,slot).B=numerator/denominator;
	    }
	}
    }
    cout<<(*map);
    return 0;
}
