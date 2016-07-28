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
	Plotter::Instance().SetOutput(".","AB-synchro");
	for(size_t layer=1;layer<=3;layer++){
		hist<double> position,sigma;
		SortedPoints<double> chisq;
		for(size_t slot=1;slot<=((layer==3)?96:48);slot++){
			auto name=LayerSlotThr(layer,slot,1);
			auto fit=SyncAB::Fit4SyncAB(ReadHist(root_filenames,name),name,thr_cnt);
			position<<point<value<double>>(double(slot),fit.pos);
			sigma<<point<value<double>>(double(slot),fit.sigma);
			chisq<<point<double>(double(slot),fit.chisq);
			cout<<layer<<"\t"<<slot<<"\t"<<1<<"\t"
			    <<fit.pos.val()<<"\t"<<fit.pos.delta()<<"\t"<<fit.sigma.val()<<"\t"<<fit.chisq<<endl;
		}
		Plot<double>().Hist(position,"Position")<<"set key on";
		Plot<double>().Hist(sigma,"Sigma")<<"set key on";
		Plot<double>().Line(chisq,"Chi^2")<<"set key on";
	}
	return 0;
}
