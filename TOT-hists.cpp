// this file is distributed under 
// MIT license
#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <JPetLargeBarrelExtensions/BarrelExtensions.h>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <Calc/convention.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
    if(argc<3){
	cerr<<"Usage: "<<argv[0]
	<<" <plot-postfix> <filename> <filename> ..."<<endl;
	return -1;
    }
    vector<string> root_filenames;
    for(int i=2;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    string postfix=argv[1];
    Plotter::Instance().SetOutput(".","TOF-hists"+postfix);
    auto map=make_JPetMap<TOT_cut>();
    for(size_t layer=1;layer<=map->layersCount();layer++){
	for(size_t slot=1;slot<=map->layerSize(layer);slot++){
	    auto action=[&root_filenames,layer,slot,&postfix](const string&letter,double*output){
		Plot<double> plot;
		plot<<"set key on"<<"set xrange [30:80]";
		for(size_t t=0;t<thresholds_count;t++){
		    string name="TOT-"+LayerSlotThr(layer,slot,t+1)+
		    letter+postfix;
		    auto shist=ReadHist(root_filenames,name);
		    plot.Hist(shist,name);
		    shist=shist.XRange(0.5,INFINITY);
		    double num=0,denom=0;
		    for(const auto&p:shist){
			num+=p.X().val()*p.Y().val();
			denom+=p.Y().val();
		    }
		    output[t]= num/denom;
		}
	    };
	    auto&item=map->item({.layer=layer,.slot=slot});
	    action("-A",item.A);
	    action("-B",item.B);
	}
    }
    cout<<(*map);
    return 0;
}
