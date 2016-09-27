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
	    Mul2<Par<0>,Func3<Gaussian,Arg<0>,Par<1>,Par<2>>>,
	    Mul2<Par<3>,Func3<Gaussian,Arg<0>,Par<4>,Par<5>>>
	> TotalFunc;
	FitFunction<DifferentialMutations<>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    return (P[0]>0)&&(P[2]>0)&&(P[3]>0)&&(P[5]>0)
	    &&(P[1]>hist.left().X().max())&&(P[4]<hist.right().X().min())
	    &&(P[4]>P[1])&&((P[4]-P[1])>(P[2]+P[5]))&&((P[4]-P[1])<((P[2]+P[5])*5.0))
	    &&((P[2]/P[5])<4.0)&&((P[5]/P[2])<4.0)
	    &&((P[0]/P[3])<7.0)&&((P[3]/P[0])<7.0);
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(500,make_shared<GenerateByGauss>()
	    <<make_pair(total,total*30.0)
	    <<make_pair((hist.left().X().min()+hist.right().X().max())/2.0,(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(1.5,2.5)
	    <<make_pair(total,total*30.0)
	    <<make_pair((hist.left().X().min()+hist.right().X().max())/2.0,(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(1.5,2.5)
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
	SortedPoints<double> totalfit([&fit](double x)->double{return fit({x});},chain);
	Plot<double>().Hist(hist,displayname).Line(totalfit,"Fit")<<"set key on"<<"set xrange [-30:30]";
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	fit.SetUncertaintyCalcDeltas(parEq(fit.ParamCount(),0.01));
	for(const auto&P:fit.ParametersWithUncertainties())cerr<<P<<endl;
	auto F=[](double x){return log(x);};
	return {
	    .position_left=fit.ParametersWithUncertainties()[1],.width_left=fit.ParametersWithUncertainties()[2],
	    .position_right=fit.ParametersWithUncertainties()[4],.width_right=fit.ParametersWithUncertainties()[5],
	    .assymetry=func_value<double>(F, fit.ParametersWithUncertainties()[3]/fit.ParametersWithUncertainties()[0] ),
	    .chi_sq=chi_sq_norm
	};
	
    }
}