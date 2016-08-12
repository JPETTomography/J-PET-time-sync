#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
    if(argc<3){
	cerr<<"Usage: "<<argv[0]<<" <thread_count> <filename> <filename> ..."<<endl;
	return -1;
    }
    string parameters_file(argv[1]);
    vector<string> root_filenames;
    for(int i=2;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    Plotter::Instance().SetOutput(".","strips-synchro");
    for(size_t slot=1;slot<=24;slot++)
	Plot<double>().Hist(ReadHist(root_filenames,"Delta_t_with_oposite_"+LayerSlotThr(1,slot,1)).Scale(4),"opo")<<"set key on";
    for(size_t slot=1;slot<=48;slot++)
	Plot<double>().Hist(ReadHist(root_filenames,"Delta_t_with_neighbour_r_"+LayerSlotThr(1,slot,1)).Scale(4),"nei")<<"set key on";
    
    return 0;
}
