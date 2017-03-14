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
    const SyncScatter_results Fit4SyncScatter(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads){
	cerr<<"=========== "<<displayname<<" ==============="<<endl;
	if(hist.TotalSum().val()<7.){
	    Plot<double>().Hist(hist)<<TIME_PLOT_OPTS
	    <<"set title'"+displayname+"'";
	    cerr<<"TOO FEW STATISTICS"<<endl;
	    return {.left=0,.right=0,.hleft=0,.hright=0,.chi_sq=-1};
	}
	double total=0;for(const auto&p:hist)total+=p.Y().val()*p.X().uncertainty()*2.0;
	typedef Mul2<Par<0>,Func3<Gaussian,Arg<0>,Par<1>,Par<2>>> Left;
	typedef Mul2<Par<3>,Func3<Gaussian,Arg<0>,Par<4>,Par<5>>> Right;
	typedef Add2<Left,Right> TotalFunc;
	FitFunction<DifferentialMutations<Uncertainty>,TotalFunc,ChiSquare> fit(make_shared<FitPoints>(hist));
	fit.SetFilter([&hist](const ParamSet&P){
	    return (P[0]>0)&&(P[3]>0)
	    &&(P[2]>0.1*TIME_UNIT_CONST)&&(P[5]>0.1*TIME_UNIT_CONST)
	    &&(P[2]<4.0*TIME_UNIT_CONST)&&(P[5]<4.0*TIME_UNIT_CONST)
	    &&(P[1]>hist.left().X().max())&&(P[4]<hist.right().X().min())
	    &&((P[4]-P[1])>(P[2]+P[5]))
	    &&((P[4]-P[1])>0.5*TIME_UNIT_CONST)&&((P[4]-P[1])<20.0*TIME_UNIT_CONST)
	    &&((P[0]/P[3])<30.0)&&((P[3]/P[0])<30.0);
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(400,make_shared<InitialDistributions>()
	    <<make_shared<DistribUniform>(0,total*30.0)
	    <<make_shared<DistribUniform>(hist.left().X().min(),hist.right().X().max())
	    <<make_shared<DistribGauss>(0.5*TIME_UNIT_CONST,0.3*TIME_UNIT_CONST)
	    <<make_shared<DistribUniform>(0,total*30.0)
	    <<make_shared<DistribUniform>(hist.left().X().min(),hist.right().X().max())
	    <<make_shared<DistribGauss>(0.5*TIME_UNIT_CONST,0.3*TIME_UNIT_CONST)
	,r);
	cerr<<fit.ParamCount()<<" parameters"<<endl;
	cerr<<fit.PopulationSize()<<" points"<<endl;
	const auto deltas=parEq(fit.ParamCount(),0.0001);
	while(
	    (!fit.AbsoluteOptimalityExitCondition(0.00001))&&
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
	SortedPoints<double> left([&fit](double x)->double{
	    static const Left L;
	    return L({x},fit.Parameters());
	},chain);
	SortedPoints<double> right([&fit](double x)->double{
	    static const Right R;
	    return R({x},fit.Parameters());
	},chain);
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	if(fit.iteration_count()>=1000){
	    Plot<double>().Hist(hist)<<TIME_PLOT_OPTS
	    <<"set title'"+displayname+"'";
	    cerr<<"TIMEOUT"<<endl;
	    return {.left=0,.right=0,.hleft=0,.hright=0,.chi_sq=-1};
	}
	const auto& P=fit.Parameters();
	const auto& Pu=fit.ParametersWithUncertainties();
	const SyncScatter_results res={
	    .left={P[1],P[2]},.right={P[4],P[5]},
	    .hleft=Pu[0],.hright=Pu[3],
	    .chi_sq=chi_sq_norm
	};
	if(res.valid()){
	    Plot<double>().Hist(hist).Line(totalfit,"Fit")
	    .Line(left).Line(right)<<"set key on"<<TIME_PLOT_OPTS
	    <<"set title'"+displayname+"'";
	    cerr<<"VALID"<<endl;
	}else{
	    Plot<double>().Hist(hist)<<TIME_PLOT_OPTS
	    <<"set title'"+displayname+"'";
	    cerr<<"NOT VALID"<<endl;
	}
	return res;
    }
}
