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
    const SyncNeighbour_results Fit4SyncNeighbour(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads){
	cerr<<"=========== "<<displayname<<" ==============="<<endl;
	double total=0;for(const auto&p:hist)total+=p.Y().val()*p.X().uncertainty()*2.0;
	typedef Add2<
	    Mul2<Par<0>,Func4<Novosibirsk,Arg<0>,Par<1>,Par<2>,Par<3>>>,
	    Mul2<Par<4>,Func4<Novosibirsk,Arg<0>,Par<5>,Par<6>,Par<7>>>
	>
	TotalFunc;
	FitFunction<DifferentialMutations<>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    return (P[0]>0)&&(P[2]>0)&&(P[4]>0)&&(P[6]>0)
	    &&(P[1]>hist.left().X().max())
	    &&((P[1]+P[2])<(P[5]-P[6]))
	    &&(P[5]<hist.right().X().min());
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(20*TotalFunc::ParamCount,make_shared<GenerateByGauss>()
	<<make_pair(total,total*5.0)
	<<make_pair(hist.left().X().min()+hist.right().X().max()/2.0,hist.right().X().max()-hist.left().X().min())
	<<make_pair(0.1,0.5)
	<<make_pair(0.0,0.1)
	<<make_pair(total,total*5.0)
	<<make_pair(hist.left().X().min()+hist.right().X().max()/2.0,hist.right().X().max()-hist.left().X().min())
	<<make_pair(0.1,0.5)
	<<make_pair(0.0,0.1)
	,r);
	cerr<<fit.ParamCount()<<" parameters"<<endl;
	cerr<<fit.PopulationSize()<<" points"<<endl;
	while(!fit.AbsoluteOptimalityExitCondition(0.0000001)){
	    fit.Iterate(r);
	    cerr<<fit.iteration_count()<<" iterations; "
	    <<fit.Optimality()<<"<S<"
	    <<fit.Optimality(fit.PopulationSize()-1)
	    <<"        \r";
	}
	auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
	SortedPoints<double> totalfit([&fit](double x)->double{return fit({x});},chain);
	Plot<double>().Hist(hist,displayname).Line(totalfit,"Fit")<<"set key on"<<"set xrange [-30:30]";
	cerr<<endl<<"done. S="<<fit.Optimality()<<endl;
	fit.SetUncertaintyCalcDeltas(parEq(fit.ParamCount(),0.01));
	for(const auto&P:fit.ParametersWithUncertainties())cerr<<P<<endl;
	return {
	    .position_left=fit.ParametersWithUncertainties()[1],.width_left=fit.ParametersWithUncertainties()[2],
	    .position_right=fit.ParametersWithUncertainties()[5],.width_right=fit.ParametersWithUncertainties()[6],
	    .chi_sq=fit.Optimality()
	};
	
    }
}