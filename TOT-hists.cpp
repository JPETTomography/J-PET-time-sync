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
	cerr<<"Usage: "<<argv[0]<<" <plot-postfix> <filename> <filename> ..."<<endl;
	return -1;
    }
    vector<string> root_filenames;
    for(int i=2;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    Plotter::Instance().SetOutput(".","TOF-hists-"+string(argv[1]));
    auto map=make_JPetMap<TOT_cut>();
    for(size_t layer=1;layer<=map->LayersCount();layer++){
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
	    auto action=[&root_filenames](const string&name,double&output){
		const auto shist=ReadHist(root_filenames,name);
		Plot<double>().Hist(shist,name)<<"set key on"<<"set xrange [30:70]";
		double numerator=0,denominator=0;
		for(const auto&point:shist)if(point.X().Above(point.X().uncertainty())){
		    numerator+=point.Y().val()*point.X().val();
		    denominator+=point.Y().val();
		}
		output=numerator/denominator;
	    };
	    auto&item=map->Item(layer,slot);
	    action("TOT_"+LayerSlotThr(layer,slot,1)+"_A",item.A);
	    action("TOT_"+LayerSlotThr(layer,slot,1)+"_B",item.B);
	}
    }
    cout<<(*map);
    return 0;
}
