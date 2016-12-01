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
    const SyncScatter_results Fit4SyncScatter(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads){
	if(hist.TotalSum().val()<7.){
	    Plot<double>().Hist(hist)<<"set xrange [-30:30]"<<"set title'"+displayname+"'";
	    return {.left=0,.right=0,.assymetry=0,.chi_sq=-1};
	}
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
	    &&((P[4]-P[1])>((P[2]+P[5])/2.0))
	    &&((P[4]-P[1])<((P[2]+P[5])*10.0))
	    &&((P[2]/P[5])<4.0)&&((P[5]/P[2])<4.0)
	    &&((P[0]/P[3])<5.0)&&((P[3]/P[0])<5.0);
	});
	fit.SetThreadCount(threads);
	RANDOM r;
	fit.Init(150,make_shared<GenerateByGauss>()
	    <<make_pair(total,total*30.0)
	    <<make_pair((hist.left().X().min()+hist.right().X().max())/2.0,(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(0.5,0.3)
	    <<make_pair(total,total*30.0)
	    <<make_pair((hist.left().X().min()+hist.right().X().max())/2.0,(hist.right().X().max()-hist.left().X().min())/2.0)
	    <<make_pair(0.5,0.3)
	,r);
	cerr<<fit.ParamCount()<<" parameters"<<endl;
	cerr<<fit.PopulationSize()<<" points"<<endl;
	const auto deltas=parEq(fit.ParamCount(),0.001);
	while(
	    (!fit.AbsoluteOptimalityExitCondition(0.001))&&
	    (!fit.ParametersDispersionExitCondition(deltas))
	){
	    fit.Iterate(r);
	    cerr<<fit.iteration_count()<<" iterations; "
	    <<fit.Optimality()<<"<chi^2<"
	    <<fit.Optimality(fit.PopulationSize()-1)
	    <<"        \r";
	}
	auto chain=ChainWithCount(1000,hist.left().X().min(),hist.right().X().max());
	SortedPoints<double> totalfit([&fit](double x)->double{return fit({x});},chain);
	Plot<double>().Hist(hist).Line(totalfit,"Fit")<<"set key on"<<"set xrange [-30:30]"<<"set title'"+displayname+"'";
	auto chi_sq_norm=fit.Optimality()/(fit.Points()->size()-fit.ParamCount());
	cerr<<endl<<"done. chi^2/D="<<chi_sq_norm<<endl;
	const auto& P=fit.Parameters();
	for(const auto&p:P)cerr<<p<<endl;
	return {.left={P[1],P[2]},.right={P[4],P[5]},.assymetry=P[3]/P[0],.chi_sq=chi_sq_norm};
    }
}
