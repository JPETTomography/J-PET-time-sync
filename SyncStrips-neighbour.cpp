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
    auto map=make_JPetMap<SyncNeighbour_results>();
    Plotter::Instance().SetOutput(".","strips-neighbour");
    for(size_t layer=1;layer<map->LayersCount();layer++){
	hist<double> positionl,sigmal,positionr,sigmar,pos_diff,assym;
	SortedPoints<double> chisq;
	for(size_t slot=1;slot<=map->LayerSize(layer);slot++){
	    auto& item=map->Item(layer,slot)=Sync::Fit4SyncNeighbour(
		ReadHist(root_filenames,"Delta_t_with_neighbour_"+LayerSlotThr(layer,slot,1)+"_deltaid3"),
		"Neighbour "+LayerSlotThr(layer,slot,1),thr_cnt
	    );
	    positionl<<point<value<double>>(double(slot),item.left);
	    sigmal<<point<value<double>>(double(slot),item.left.uncertainty());
	    positionr<<point<value<double>>(double(slot),item.right);
	    sigmar<<point<value<double>>(double(slot),item.right.uncertainty());
	    pos_diff<<point<value<double>>(double(slot), item.right - item.left );
	    assym<<point<value<double>>(double(slot),item.assymetry);
	    chisq<<point<double>(double(slot),item.chi_sq);
	}
	Plot<double>().Hist(positionl,"Position Left").Hist(positionr,"Position Right")<<"set key on";
	Plot<double>().Hist(sigmal,"Sigma Left").Hist(sigmar,"Sigma Right")<<"set key on"<<"set yrange [0:]";
	Plot<double>().Hist(pos_diff,"Distance between peaks")<<"set key on"<<"set yrange [0:]";
	Plot<double>().Hist(assym,"Assymetry of peaks height")<<"set key on"<<"set yrange [0:]";
	Plot<double>().Line(chisq,"Chi^2")<<"set key on"<<"set yrange [0:]";
    }
    cout<<(*map);
    return 0;
}
