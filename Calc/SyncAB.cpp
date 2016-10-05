// this file is distributed under 
// MIT license
#include <iostream>
#include <gnuplot_wrap.h>
#include <math_h/functions.h>
#include <Genetic/fit.h>
#include <Genetic/initialconditions.h>
#include <Genetic/filter.h>
#include "SyncProcedures.h"
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
using namespace Genetic;
namespace Sync{
    const SyncAB_results Fit4SyncAB(const hist<double>&hist, const string&displayname,const size_t threads){
	cerr<<"=========== "<<displayname<<" ==============="<<endl;
	double total=0;for(const auto&p:hist)total+=p.Y().val()*p.X().uncertainty()*2.0;
	typedef 
	    Mul2<Par<0>,Func3<Gaussian ,Arg<0>,Par<1>,Par<2>>> 
	Foreground;
	typedef 
	    Mul3<
		Func3<FermiFunc,Arg<0>,Par<3>,Par<4>>,
		Func3<FermiFunc,Arg<0>,Par<5>,Par<6>>,
		PolynomFunc<0,7,1>
	    >
	Background;
	typedef Add<Foreground,Background> TotalFunc;
	FitFunction<DifferentialMutations<>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    return (P[0]>0)&&(P[2]>0)&&(P[4]<0)&&(P[6]>0)
	    &&(-(P[4]/P[6])<5.0)&&(-(P[6]/P[4])<4.0)
	    &&(P[3]<P[1])&&(P[5]>P[1])
	    &&(P[3]>hist.left().X().max())&&(P[5]<hist.right().X().min());
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(1000,make_shared<GenerateByGauss>()
	    <<make_pair(total,total*30.0)
	    <<make_pair((hist.left().X().min()+hist.right().X().max())/2.0,(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(1.5,2.5)
	    <<make_pair(hist.left().X().max(),(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(-0.5,0.5)
	    <<make_pair(hist.right().X().min(),(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(0.5,0.5)
	    <<make_pair(total,total*20.0)
	    <<make_pair(0.0,5.0)
	,r);
	cerr<<fit.ParamCount()<<" parameters"<<endl;
	cerr<<fit.PopulationSize()<<" points"<<endl;
	while(!fit.AbsoluteOptimalityExitCondition(0.00001)){
	    fit.Iterate(r);
	    cerr<<fit.iteration_count()<<" iterations; "
	    <<fit.Optimality()<<"<chi^2<"
	    <<fit.Optimality(fit.PopulationSize()-1)
	    <<"        \r";
	}
	auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
	SortedPoints<double>
	totalfit([&fit](double x)->double{return fit({x});},chain),
	background([&fit](double x)->double{return Background()({x},fit.Parameters());},chain);
	Plot<double>().Hist(hist,displayname).Line(totalfit,"Fit").Line(background,"Background")<<"set key on"<<"set xrange [-30:30]";
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	fit.SetUncertaintyCalcDeltas(parEq(fit.ParamCount(),0.0001));
	const auto&P=fit.ParametersWithUncertainties();
	for(const auto&p:P)cerr<<p<<endl;
	return {.position=P[1],.width=P[2],.chi_sq=chi_sq_norm};
    }
    
};
