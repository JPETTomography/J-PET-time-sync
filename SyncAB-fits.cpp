#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
#include <Calc/SyncAB.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
	if(argc==1){
		cout<<"filename list is requires as parameters"<<endl;
		return -1;
	}
	vector<string> root_filenames;
	for(int i=1;i<argc;i++)
		root_filenames.push_back(string(argv[i]));
	Plotter::Instance().SetOutput(".","test");
	for(size_t layer=1;layer<=3;layer++){
		hist<double> position,sigma;
		SortedPoints<double> chisq;
		for(size_t slot=1;slot<=((layer==3)?96:48);slot++){
			auto name=LayerSlotThr(layer,slot,1);
			cout<<"=========== "<<name<<" ==============="<<endl;
			auto hist=ReadHist(root_filenames,{"Stats"},name);
			auto fit=SyncAB::Fit4SyncAB(hist,name);
			position<<point<value<double>>(double(slot),fit.pos);
			sigma<<point<value<double>>(double(slot),fit.sigma);
			chisq<<point<double>(double(slot),fit.chisq);
		}
		Plot<double>().Hist(position,"Position")<<"set key on";
		Plot<double>().Hist(sigma,"Sigma")<<"set key on";
		Plot<double>().Line(chisq,"Chi^2")<<"set key on";
	}
	return 0;
}
