// this file is distributed under 
// MIT license
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <map>
#include <gnuplot_wrap.h>
#include <Genetic/equation2.h>
#include <Genetic/genetic.h>
#include <Genetic/initialconditions.h>
#include <Genetic/parabolic.h>
#include <IO/gethist.h>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <JPetLargeBarrelExtensions/TimeSyncDeltas.h>
#include <Calc/SyncProcedures.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
using namespace Genetic;
using namespace Graph;
int main(int argc, char **argv) {
    cerr<<"=========== SOLVING EQUATIONS ==============="<<endl;
    RANDOM engine;
    if(argc!=4){
	cerr<<"Usage: "<<argv[0]
	<<" <AB.txt> <Oposite.txt> <Scattered.txt>"<<endl;
	return -1;
    }
    vector<string> filenames;
    for(int i=1;i<argc;i++)
	filenames.push_back(string(argv[i]));
    const auto AB=make_JPetMap<SyncAB_results>();{
	ifstream file;
	file.open(filenames[0]);
	if(file){
	    file>>(*AB);file.close();
	}
    }
    const auto Opo=make_OpoCoiMap();{
	ifstream file;
	file.open(filenames[1]);
	if(file){
	    file>>(*Opo);file.close();
	}
    }
    vector<shared_ptr<JPetMap<SyncScatter_results>>> Nei;
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
	Nei.push_back(make_JPetMap<SyncScatter_results>());
    auto IL=make_InterLayerMap();{
	ifstream file;
	file.open(filenames[2]);
	if(file){
	    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
		file>>(*Nei[i]);
	    file>>(*IL);
	    file.close();
	}
    }
    const auto DeltaT=make_JPetMap<SynchroStrip>();
    cin>>(*DeltaT);
    Plotter::Instance().SetOutput(".","Delta");


    const auto totalN=DeltaT->slotsCount();
    list<InexactEquation> equations;
    ConnectionChecker graph(totalN);
    hist<double> conn_plot;
    for(size_t L=1;L<=DeltaT->layersCount();L++){
	const size_t N=DeltaT->layerSize(L);
	for(size_t i1=0;i1<(N/2);i1++){
	    const auto i2=i1+(N/2);
	    const auto&Item=Opo->operator[]({.layer=L,.slot=i1+1});
	    const StripPos pos1={.layer=L,.slot=i1+1};
	    const StripPos pos2={.layer=L,.slot=i2+1};
	    const auto gl1=DeltaT->globalSlotNumber(pos1);
	    const auto gl2=DeltaT->globalSlotNumber(pos2);
	    if(
		AB->operator[](pos1).valid()&&
		AB->operator[](pos2).valid()&&
		Item.valid()
	    ){
		equations.push_back({
		    .left=[gl1,gl2](const ParamSet&delta){
			return delta[gl2]-delta[gl1];
		    },
		    .right=Item.peak
		});
		graph.Connect(gl1,gl2);
		conn_plot<<point<value<double>>(gl1,gl2);
	    }
	}
	for(size_t i1=0;i1<N;i1++){
	    const StripPos pos1={.layer=L,.slot=i1+1};
	    const auto gl1=DeltaT->globalSlotNumber(pos1);
	    if(AB->operator[](pos1).valid())
	    for(size_t di=0;di<neighbour_delta_id.size();di++){
		const auto&Item=Nei[di]->operator[](pos1);
		const auto DI=neighbour_delta_id[di];
		const auto i2=(i1+DI)%N;
		const StripPos pos2={.layer=L,.slot=i2+1};
		const auto gl2=DeltaT->globalSlotNumber(pos2);
		if(
		    AB->operator[](pos2).valid()&&Item.valid()
		){
		    equations.push_back({
			.left=[gl1,gl2](const ParamSet&delta){
			    return delta[gl2]-delta[gl1];
			},
			.right=(Item.left+Item.right)/2.0
		    });
		    graph.Connect(gl1,gl2);
		    conn_plot<<point<value<double>>(gl1,gl2);
		}
	    }
	}
	if(L<=IL->layersCount()){
	    for(size_t i1=0;i1<N;i1++){
		const StripPos pos1={.layer=L,.slot=i1+1};
		auto process=[&pos1,&i1,&L,&N,&AB,&DeltaT,&equations,&graph,&conn_plot](
		    const SyncScatter_results&Item,const size_t coinc_index
		){
		    const auto&item=SyncLayerIndices[L-1][coinc_index];
		    const StripPos pos2={
			.layer=L+1,
			.slot=((i1*item.coef+item.offs)%
			    DeltaT->layerSize(L+1))+1
		    };
		    const auto gl1=DeltaT->globalSlotNumber(pos1);
		    const auto gl2=DeltaT->globalSlotNumber(pos2);
		    if(
			AB->operator[](pos1).valid()&&AB->operator[](pos2).valid()&&Item.valid()
		    ){
			equations.push_back({
			    .left=[gl1,gl2](const ParamSet&delta){
				return delta[gl2]-delta[gl1];
			    },
			    .right=(Item.left+Item.right)/2.0
			});
			graph.Connect(gl1,gl2);
			conn_plot<<point<value<double>>(gl1,gl2);
		    }
		};
		process(IL->operator[](pos1).zero,0);
		process(IL->operator[](pos1).one,1);
	    }
	}
    }
    Plot<double>().Hist(conn_plot);
    vector<size_t> connected;{
	vector<size_t>look;
	for(size_t grouphead=0;grouphead<totalN;grouphead++){
	    bool isnew=true;
	    for(const size_t i:look)if(i==grouphead)isnew=false;
	    if(isnew){
		vector<size_t> group=graph.connected_to(grouphead);
		cerr<<"Connected group of "<<group.size()<<" variables found"<<endl;
		for(const size_t i:group)look.push_back(i);
		if(connected.size()<group.size()){
		    connected.clear();
		    for(const size_t i:group)connected.push_back(i);
		}
	    }
	}
    }
    cerr<<equations.size()<<" equations connect "
	<<connected.size()<<" of "<<totalN<<" variables"<<endl;
    InexactEquationSolver<AbsoluteMutations<ParabolicErrorEstimationFromChisq>> solver_hits(equations);
    auto init=make_shared<InitialDistributions>();
    ParamSet deltas;
    for(size_t i=0;i<totalN;i++){
	init<<make_shared<DistribGauss>(0,SOLVING_EQ_PARAM_SIGMA);
	deltas<<0.01;
    }
    solver_hits.SetThreadCount(1);
    solver_hits.SetAbsoluteMutationsProbability(1.);
    solver_hits.Init(init->Count(),init,engine);
    cerr<<"Genetic algorithm:"<<endl;
    cerr<<solver_hits.PopulationSize()<<" points"<<endl;
    SortedPoints<double> opt_min,opt_max;
    double d_max=SOLVING_EQ_PARAM_SIGMA*3.0;
    Plot<double> variables;
    variables<<"set xlabel 'global slot index'"
    <<OUT_PLOT_OPTS;
    while(
	(d_max>0.01*TIME_UNIT_CONST)
    ){
	if((solver_hits.iteration_count()%500)==1){
	    hist<double> delta_hits;
	    const auto&PP=solver_hits.ParametersStatistics();
	    for(size_t i=0;i<totalN;i++)
		delta_hits<<point<value<double>>(double(i),PP[i]);
	    variables.Hist(delta_hits);
	}
	ParamSet M;
	auto &min=solver_hits.Optimality(),
            &max=solver_hits.Optimality(solver_hits.PopulationSize()-1);
	const double mc=
	    (SOLVING_EQ_MUTATIONS*sqrt(max)*min/((max-min)*d_max))
	    *exp(-double(solver_hits.iteration_count())/SOLVING_EQ_MUTATIONS_TAU);
	for(size_t i=0;i<totalN;i++)
	     M<<mc;
	solver_hits.SetAbsoluteMutationCoefficients(M);
	solver_hits.Iterate(engine);
	cerr<<solver_hits.iteration_count()<<": "<<min<<"<chi^2<"<<max<<"; ";
	opt_min << point<double>(solver_hits.iteration_count(),min);
	opt_max << point<double>(solver_hits.iteration_count(),max);
	d_max=0;
	for(const auto&p:solver_hits.ParametersStatistics())
	    if(p.uncertainty()>d_max)d_max=p.uncertainty();
	cerr<<"D="<<d_max<<";M="<<mc<<"                 \r";
        solver_hits.Iterate(engine);
    }
    cerr<<endl;
    cerr<<solver_hits.iteration_count()<<": chi^2 = "<<solver_hits.Optimality()<<";";
    cerr<<endl;
    Plot<double>().Line(opt_min,"").Line(opt_max,"")
    <<"set log y"<<"set xlabel 'Iteration index'";
    Plotter::Instance()<<"unset log y";
    cerr<<"chi^2/D = "<<solver_hits.Optimality()/(equations.size()-solver_hits.ParamCount())<<endl;
    solver_hits.SetUncertaintyCalcDeltas(deltas);
    const auto&P=solver_hits.Parameters();
    const auto&PP=solver_hits.ParametersWithUncertainties();
    hist<double> eq_left,eq_right,delta_hits;
    for(size_t i=0;i<totalN;i++)
	delta_hits<<point<value<double>>(double(i),PP[i]);
    for(const auto&eq:equations){
	static size_t i=0;
	eq_left<<point<value<double>>(double(i),eq.left(P));
	eq_right<<point<value<double>>(double(i),eq.right);
	i++;
    }
    Plot<double>().Hist(eq_left,"left").Hist(eq_right,"right")<<"set key on"
    <<"set xlabel 'equation index'"<<"unset log y"<<OUT_PLOT_OPTS;
    Plot<double>().Hist(delta_hits)<<"set xlabel 'global slot index'"
    <<OUT_PLOT_OPTS;
    for(size_t i=0;i<totalN;i++){
	const StripPos slot=DeltaT->positionOfGlobalNumber(i);
	const auto& ab=AB->operator[](slot).peak.val();
	const auto& dab=AB->operator[](slot).uncertainty_estimation;
	auto&Delta=DeltaT->item(slot);
	Delta.A+=P[i]-(ab/2.0);
	Delta.B+=P[i]+(ab/2.0);
	Delta.dA=Delta.dB=sqrt(pow(PP[i].uncertainty(),2)+(dab*dab));
    }

    cout<<(*DeltaT);
}
