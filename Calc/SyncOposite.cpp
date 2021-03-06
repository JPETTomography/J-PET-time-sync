// this file is distributed under 
// MIT license
#include <iostream>
#include <gnuplot_wrap.h>
#include <math_h/functions.h>
#include <Genetic/fit.h>
#include <Genetic/initialconditions.h>
#include <Genetic/filter.h>
#include <Genetic/parabolic.h>
#include "SyncProcedures.h"
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
using namespace Genetic;
namespace Sync{
    const SyncOposite_results Fit4SyncOposite(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads){
	if(hist.TotalSum().val()<10.){
	    Plot<double>().Hist(hist)<<TIME_PLOT_OPTS<<"set title'"+displayname+"'";
	    cerr<<"TOO FEW STATISTICS"<<endl;
	    return {.peak=0,.height=0,.chi_sq=-1};
	}
	cerr<<"=========== "<<displayname<<" ==============="<<endl;
	double total=0;for(const auto&p:hist)total+=p.Y().val()*p.X().uncertainty()*2.0;
	typedef 
	    Mul2<Par<0>,Func3<Gaussian ,Arg<0>,Par<1>,Par<2>>> 
	TotalFunc;
	FitFunction<DifferentialMutations<Uncertainty>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    return (P[0]>0)&&
	    (P[2]>0.1*TIME_UNIT_CONST)&&
	    (P[2]<2.0*TIME_UNIT_CONST)&&
	    (P[1]>hist.left().X().max())&&(P[1]<hist.right().X().min());
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(300,make_shared<InitialDistributions>()
	    <<make_shared<DistribUniform>(0,30.0*total)
	    <<make_shared<DistribUniform>(hist.left().X().min(),hist.right().X().max())
	    <<make_shared<DistribGauss>(0.5*TIME_UNIT_CONST,0.2*TIME_UNIT_CONST)
	,r);
	cerr<<fit.ParamCount()<<" parameters"<<endl;
	cerr<<fit.PopulationSize()<<" points"<<endl;
	const auto deltas=parEq(fit.ParamCount(),0.0001);
	while(
	    (!fit.AbsoluteOptimalityExitCondition(0.0001))&&
	    (!fit.ParametersDispersionExitCondition(deltas))&&
	    (fit.iteration_count()<1000)
	){
	    fit.Iterate(r);
	    cerr<<fit.iteration_count()<<" iterations; "
	    <<fit.Optimality()<<"<chi^2<"
	    <<fit.Optimality(fit.PopulationSize()-1)
	    <<"        \r";
	}
	auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
	SortedPoints<double> totalfit([&fit](double x)->double{return fit({x});},chain);
	Plot<double>().Hist(hist).Line(totalfit,"Fit")
	<<"set key on"<<TIME_PLOT_OPTS<<"set title'"+displayname+"'";
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	if(fit.iteration_count()>=1000){
	    cerr<<"TIMEOUT"<<endl;
	    return {.peak=0,.height=0,.chi_sq=-1};
	}
	cerr<<"VALID"<<endl;
	fit.SetUncertaintyCalcDeltas({0.01});
	const auto&P=fit.Parameters();
	const auto&Pu=fit.ParametersWithUncertainties();
	return {.peak={P[1],P[2]},.height=Pu[0],.chi_sq=chi_sq_norm};
    }
}
