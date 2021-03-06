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
    const SyncAB_results Fit4SyncAB(const hist<double>&hist, const string&displayname,const size_t threads){
	if(hist.TotalSum().val()<100.){
	    Plot<double>().Hist(hist)<<"set title'"+displayname+"'"<<TIME_PLOT_OPTS;
	    cerr<<"TOO FEW STATISTICS"<<endl;
	    return {.peak=0,.height=0,.chi_sq=-1,.uncertainty_estimation=0.};
	}
	cerr<<"=========== "<<displayname<<" ==============="<<endl;
	double total=0;for(const auto&p:hist)
	    total+=p.Y().val()*p.X().uncertainty()*2.0;
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
	FitFunction<
	    DifferentialMutations<ParabolicErrorEstimationFromChisq>,
	    TotalFunc
	> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    static const Background bg_test;
	    return (P[0]>0)&&(P[4]<0)&&(P[6]>0)
	    &&(P[2]>0)&&(P[2]<3.0*TIME_UNIT_CONST)
	    &&(-(P[4]/P[6])<5.0)&&(-(P[6]/P[4])<4.0)
	    &&((P[3]+P[4])<(P[1]-P[2]))&&((P[1]+P[2])<(P[5]-P[6]))
	    &&(P[3]>hist.left().X().max())&&(P[5]<hist.right().X().min())
	    &&(bg_test({P[3]},P)>0)&&(bg_test({P[5]},P)>0);
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(300,make_shared<InitialDistributions>()
	    <<make_shared<DistribUniform>(0,total*40.0)
	    <<make_shared<DistribUniform>(hist.left().X().min(),hist.right().X().max())
	    <<make_shared<DistribGauss>(0.5*TIME_UNIT_CONST,0.3*TIME_UNIT_CONST)
	    <<make_shared<DistribUniform>(hist.left().X().min(),hist.right().X().max())
	    <<make_shared<DistribGauss>(-0.5*TIME_UNIT_CONST,0.5*TIME_UNIT_CONST)
	    <<make_shared<DistribUniform>(hist.left().X().min(),hist.right().X().max())
	    <<make_shared<DistribGauss>(0.5*TIME_UNIT_CONST,0.5*TIME_UNIT_CONST)
	    <<make_shared<DistribUniform>(0,total*30.0/TIME_UNIT_CONST)
	    <<make_shared<DistribGauss>(0.0,10.0/(TIME_UNIT_CONST*TIME_UNIT_CONST))
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
	fit.SetUncertaintyCalcDeltas({1,0.01});
	auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
	SortedPoints<double>
	totalfit([&fit](double x)->double{return fit({x});},chain),
	background([&fit](double x)->double{return Background()({x},fit.Parameters());},chain);
	Plot<double>().Hist(hist).Line(totalfit,"Fit").Line(background,"Background")
	<<"set key on"<<TIME_PLOT_OPTS<<"set title'"+displayname+"'";
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	if(fit.iteration_count()>=1000){
	    cerr<<"TIMEOUT"<<endl;
	    return {.peak=0,.height=0,.chi_sq=-1};
	}
	const auto&P=fit.Parameters();
	const auto&Pu=fit.ParametersWithUncertainties();
	cerr<<"VALID"<<endl;
	return {
	    .peak={P[1],P[2]},.height=Pu[0],.chi_sq=chi_sq_norm,
	    .uncertainty_estimation=Pu[1].uncertainty()
	};
    }
};
