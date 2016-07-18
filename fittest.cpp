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
	Plotter::Instance().SetOutput(".","test");
	for(size_t layer=1;layer<=3;layer++){
		hist<double> position,sigma;
		SortedPoints<double> chisq;
		for(size_t slot=1;slot<=((layer==3)?96:48);slot++){
			stringstream histname,histdir,display;
			display<<"Layer "<<layer<<" ; Slot "<<slot;
			cout<<"=========== "<<display.str()<<" ==============="<<endl;
			histname<<"TimeDiff_Slot"<<slot<<"_Layer"<<layer<<"_AB_Th1";
			histdir<<"Layer_"<<layer<<"_timeDiffAB";
			auto hist=ReadHist("Go4AutoSave.root",{"Histograms",histdir.str()},histname.str()).Scale(4);
			auto fit=SyncAB::Fit4SyncAB(hist,display.str());
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
