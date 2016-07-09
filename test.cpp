#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <math_h/functions.h>
#include <Genetic/fit.h>
#include <Genetic/initialconditions.h>
#include <Genetic/filter.h>
#include <io/gethist.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
using namespace Genetic;
int main(int argc, char **argv) {
	Plotter::Instance().SetOutput(".","test");
	for(size_t slot=1;slot<=48;slot++){
		stringstream histname;
		histname<<"TimeDiff_Slot"<<slot<<"_Layer1_AB_Th1";
		auto hist=ReadHist("Go4AutoSave.root",{"Histograms","Layer_1_timeDiffAB"},histname.str());
		double total=0;for(const auto&p:hist)total+=p.Y().val();
		typedef Mul2<Par<0>,Func3<Gaussian ,Arg<0>,Par<1>,Par<2>>> Foreground;
		typedef Mul3<Par<3>,Func3<FermiFunc,Arg<0>,Par<4>,Par<5>>
		                   ,Func3<FermiFunc,Arg<0>,Par<6>,Par<7>>> Background;
		typedef Add<Foreground,Background> TotalFunc;
		FitFunction<DifferentialMutations<>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
		fit.SetFilter([](const ParamSet&P){
			return (P[0]>0)&&(P[2]>0)&&(P[3]>0)&&(P[5]<0)&&(P[7]>0)&&(P[4]<P[6]);
		});
		fit.SetThreadCount(2);
		RANDOM r;
		fit.Init(200,make_shared<GenerateUniform>()
			<<make_pair(0.0,total)
			<<make_pair(hist.left().X().min(),hist.right().X().max())
			<<make_pair(0.0,hist.right().X().max()-hist.left().X().min())
			<<make_pair(0.0,total)
			<<make_pair(hist.left().X().min(),hist.right().X().max())
			<<make_pair((hist.left().X().min()-hist.right().X().max())/10.0,0.0)
			<<make_pair(hist.left().X().min(),hist.right().X().max())
			<<make_pair(0.0,(hist.right().X().max()-hist.left().X().min())/10.0)
		,r);
		while(!fit.AbsoluteOptimalityExitCondition(0.0001))
			fit.Iterate(r);
		auto chain=ChainWithStep(hist.left().X().min(),0.5,hist.right().X().max());
		SortedPoints<double>
			totalfit([&fit](double x)->double{return fit({x});},chain),
			background([&fit](double x)->double{return Background()({x},fit.Parameters());},chain);
		Plot<double>().Hist(hist,"").Line(totalfit,"Fit").Line(background,"Background");
	}
	return 0;
}
