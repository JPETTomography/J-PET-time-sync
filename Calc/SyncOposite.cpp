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
    const SyncOposite_results Fit4SyncOposite(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads){
	cerr<<"=========== "<<displayname<<" ==============="<<endl;
	double total=0;for(const auto&p:hist)total+=p.Y().val()*p.X().uncertainty()*2.0;
	typedef 
	    Mul2<Par<0>,Func3<Gaussian ,Arg<0>,Par<1>,Par<2>>> 
	TotalFunc;
	FitFunction<DifferentialMutations<>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    return (P[0]>0)&&(P[2]>0)&&(P[1]>hist.left().X().max())&&(P[1]<hist.right().X().min());
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(100,make_shared<GenerateByGauss>()
	    <<make_pair(total,total*30.0)
	    <<make_pair((hist.left().X().min()+hist.right().X().max())/2.0,(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(1.5,2.5)
	,r);
	cerr<<fit.ParamCount()<<" parameters"<<endl;
	cerr<<fit.PopulationSize()<<" points"<<endl;
	ParamSet pExit{	1,	0.01,	0.001	},
		pDelta{	0.1,	0.001,	0.001	};
	while((!fit.AbsoluteOptimalityExitCondition(0.0001))&&(!fit.ParametersDispersionExitCondition(pExit))){
	    fit.Iterate(r);
	    cerr<<fit.iteration_count()<<" iterations; "
	    <<fit.Optimality()<<"<chi^2<"
	    <<fit.Optimality(fit.PopulationSize()-1)
	    <<"        \r";
	}
	auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
	SortedPoints<double> totalfit([&fit](double x)->double{return fit({x});},chain);
	Plot<double>().Hist(hist,displayname).Line(totalfit,"Fit")<<"set key on"<<"set xrange [-30:30]";
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	fit.SetUncertaintyCalcDeltas(pDelta);
	for(const auto&p:fit.ParametersWithUncertainties())cerr<<p<<endl;
	const auto& P=fit.Parameters();
	return {.peak={P[1],P[2]},.chi_sq=chi_sq_norm};
	
    }
}