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
    string postfix=argv[1];
    Plotter::Instance().SetOutput(".","TOF-hists"+postfix);
    auto map=make_JPetMap<TOT_cut>();
    for(size_t layer=1;layer<=map->LayersCount();layer++){
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
	    auto action=[&root_filenames](const string&name,double&output){
		auto shist=ReadHist(root_filenames,name);
		Plot<double>().Hist(shist,name)<<"set key on"<<"set xrange [30:70]";
		shist=shist.XRange(0.2,INFINITY);
		double max=0;for(const auto&p:shist)if(max<p.Y().val())max=p.Y().val();
		shist=shist.YRange(max*0.1,INFINITY);
		output=(shist.left().X()+shist.right().X()).val()/2.0;
	    };
	    auto&item=map->Item(layer,slot);
	    action("TOT-"+LayerSlotThr(layer,slot,1)+"-A"+postfix,item.A);
	    action("TOT-"+LayerSlotThr(layer,slot,1)+"-B"+postfix,item.B);
	}
    }
    cout<<(*map);
    return 0;
}
