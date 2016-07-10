// this file is distributed under 
// MIT license
#include <iostream>
#include <gnuplot_wrap.h>
#include <math_h/functions.h>
#include <Genetic/fit.h>
#include <Genetic/initialconditions.h>
#include <Genetic/filter.h>
#include "SyncAB.h"
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
using namespace Genetic;
namespace SyncAB{
	const pair<value<double>,double> Fit4SyncAB(const hist<double>&hist){
		double total=0;for(const auto&p:hist)total+=p.Y().val()*p.X().delta()*2.0;
		typedef 
			Mul2<Par<0>,Func3<Gaussian ,Arg<0>,Par<1>,Par<2>>> 
		Foreground;
		typedef 
			Mul3<
				Func3<FermiFunc,Arg<0>,Par<3>,Par<4>>,
				Func3<FermiFunc,Arg<0>,Par<5>,Par<6>>,
				PolynomFunc<0,7,0>//the power can be increased
			>
		Background;
		typedef Add<Foreground,Background> TotalFunc;
		FitFunction<DifferentialMutations<>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
		fit.SetFilter([&hist](const ParamSet&P){
			return (P[0]>0)&&(P[2]>0)&&(P[7]>0)&&(P[4]<0)&&(P[6]>0)
			&&(P[3]<(P[1]-P[2]))&&(P[5]>(P[1]+P[2]))&&(P[0]>P[7])
			&&(-(P[4]/P[6])<5.0)&&(-(P[6]/P[4])<5.0)
			&&(P[3]>hist.left().X().max())&&(P[5]<hist.right().X().min())
			&&(P[1]>hist.left().X().max())&&(P[1]<hist.right().X().min());
		});
		fit.SetThreadCount(2);
		RANDOM r;
		fit.Init(10*TotalFunc::ParamCount,make_shared<GenerateByGauss>()
			<<make_pair(total,total*12.0)
			<<make_pair(hist.left().X().min()+hist.right().X().max()/2.0,hist.right().X().max()-hist.left().X().min())
			<<make_pair(0.1,0.5)
			<<make_pair(hist.left().X().max(),hist.right().X().max()-hist.left().X().min())
			<<make_pair(-0.5,0.5)
			<<make_pair(hist.right().X().min(),hist.right().X().max()-hist.left().X().min())
			<<make_pair(0.5,0.5)
			<<make_pair(total,total*10.0)
		,r);
		cout<<fit.ParamCount()<<" parameters"<<endl;
		cout<<fit.PopulationSize()<<" points"<<endl;
		while(!fit.AbsoluteOptimalityExitCondition(0.0001)){
			fit.Iterate(r);
			cout<<fit.iteration_count()<<" iterations; "
				<<fit.Optimality()<<"<S<"
				<<fit.Optimality(fit.PopulationSize()-1)
				<<"        \r";
		}
		auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
		SortedPoints<double>
		totalfit([&fit](double x)->double{return fit({x});},chain),
		background([&fit](double x)->double{return Background()({x},fit.Parameters());},chain);
		Plot<double>().Hist(hist,"").Line(totalfit,"Fit").Line(background,"Background")<<"set key on";
		cout<<endl<<"done. S="<<fit.Optimality()<<endl;
		fit.SetUncertaintyCalcDeltas(parEq(fit.ParamCount(),0.01));
		for(const auto&P:fit.ParametersWithUncertainties())cout<<P<<endl;
		return make_pair(fit.ParametersWithUncertainties()[1],fit.Optimality());
	}

};
