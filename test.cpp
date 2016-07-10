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
	hist<double> position;
	SortedPoints<double> chisq;
	for(size_t slot=1;slot<=48;slot++){
		cout<<"=========== Slot "<<slot<<" ==============="<<endl;
		stringstream histname;
		histname<<"TimeDiff_Slot"<<slot<<"_Layer1_AB_Th1";
		auto hist=ReadHist("Go4AutoSave.root",{"Histograms","Layer_1_timeDiffAB"},histname.str()).Scale(4);
		auto fit=SyncAB::Fit4SyncAB(hist);
		position<<point<value<double>>(double(slot),fit.first);
		chisq<<point<double>(double(slot),fit.second);
	}
	Plot<double>().Hist(position,"");
	Plot<double>().Line(chisq,"");
	return 0;
}
