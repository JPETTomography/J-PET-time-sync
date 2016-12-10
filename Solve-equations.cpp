#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <Genetic/equation2.h>
#include <Genetic/genetic.h>
#include <Genetic/initialconditions.h>
#include <IO/gethist.h>
#include <LargeBarrelExtensions/PetDict.h>
#include <LargeBarrelExtensions/TimeSyncDeltas.h>
#include <Calc/SyncProcedures.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
using namespace Genetic;
using namespace Graph;
int main(int argc, char **argv) {
    cerr<<"=========== SOLVING EQUATIONS ==============="<<endl;
    RANDOM engine;
    if(argc!=5){
	cerr<<"Usage: "<<argv[0]<<" <thread_count> <AB.txt> <Oposite.txt> <Nieghbour.txt>"<<endl;
	return -1;
    }
    size_t thr_cnt=0;
    {
	stringstream thr_count(argv[1]);
	thr_count>>thr_cnt;
	if(0==thr_cnt){
	    cerr<<"Wrong threads count"<<endl;
	    return -1;
	}
    }
    vector<string> filenames;
    for(int i=2;i<argc;i++)
	filenames.push_back(string(argv[i]));
    const auto AB=make_JPetMap<SyncAB_results>();
    {ifstream file;file.open(filenames[0]);if(file){file>>(*AB);file.close();}}
    const auto Opo=make_OpoCoiMap();
    {ifstream file;file.open(filenames[1]);if(file){file>>(*Opo);file.close();}}
    vector<shared_ptr<JPetMap<SyncScatter_results>>> Nei;
    for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
	Nei.push_back(make_JPetMap<SyncScatter_results>());
    auto IL=make_InterLayerMap();
    {ifstream file;file.open(filenames[2]);if(file){
	for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
	    file>>(*Nei[i]);
	file>>(*IL);
	file.close();
    }}
    const auto DeltaT=make_JPetMap<SynchroStrip>();
    cin>>(*DeltaT);
    Plotter::Instance().SetOutput(".","Delta");


    const auto totalN=DeltaT->SlotsCount();
    list<InexactEquation> equations;
    ConnectionChecker graph(totalN);

    for(size_t L=1;L<=DeltaT->LayersCount();L++){
	const size_t N=DeltaT->LayerSize(L);
	for(size_t i1=0;i1<(N/2);i1++){
	    const auto i2=i1+(N/2);
	    const auto&Item=Opo->operator[]({.layer=L,.slot=i1+1});
	    const StripPos pos1={.layer=L,.slot=i1+1};
	    const StripPos pos2={.layer=L,.slot=i2+1};
	    const auto gl1=DeltaT->GlobalSlotNumber(pos1);
	    const auto gl2=DeltaT->GlobalSlotNumber(pos2);
	    if(
		AB->operator[](pos1).valid()&&
		AB->operator[](pos2).valid()&&
		Item.valid()
	    ){
		equations.push_back({
		    .left=[gl1,gl2](const ParamSet&delta){return delta[gl2]-delta[gl1];},
		    .right=Item.peak
		});
		graph.Connect(gl1,gl2);
	    }
	}
	for(size_t i1=0;i1<N;i1++){
	    const StripPos pos1={.layer=L,.slot=i1+1};
	    const auto gl1=DeltaT->GlobalSlotNumber(pos1);
	    if(AB->operator[](pos1).valid())
	    for(size_t di=0;di<neighbour_delta_id.size();di++){
		const auto&Item=Nei[di]->operator[](pos1);
		const auto DI=neighbour_delta_id[di];
		const auto i2=(i1+DI)%N;
		const StripPos pos2={.layer=L,.slot=i2+1};
		const auto gl2=DeltaT->GlobalSlotNumber(pos2);
		if(
		    AB->operator[](pos2).valid()&&Item.valid()&&
		    (Item.assymetry<=2)&&(Item.assymetry>=0.5)&&
		    (Item.right-Item.left).Below(5.0*double(DI))
		){
		    equations.push_back({
			.left=[gl1,gl2](const ParamSet&delta){return delta[gl2]-delta[gl1];},
			.right=(Item.left+Item.right)/2.0
		    });
		    graph.Connect(gl1,gl2);
		}
	    }
	}
	if(L<=IL->LayersCount()){
	    for(size_t i1=0;i1<N;i1++){
		const StripPos pos1={.layer=L,.slot=i1+1};
		auto process=[&pos1,&i1,&L,&N,&AB,&DeltaT,&equations,&graph](const SyncScatter_results&Item,const size_t coinc_index){
		    const auto&item=SyncLayerIndices[L-1][coinc_index];
		    const StripPos pos2={.layer=L+1,.slot=((i1*item.coef+item.offs)%DeltaT->LayerSize(L+1))+1};
		    const auto gl1=DeltaT->GlobalSlotNumber(pos1);
		    const auto gl2=DeltaT->GlobalSlotNumber(pos2);
		    if(
			AB->operator[](pos1).valid()&&AB->operator[](pos2).valid()&&Item.valid()
		    ){
			equations.push_back({
			    .left=[gl1,gl2](const ParamSet&delta){return delta[gl2]-delta[gl1];},
			    .right=(Item.left+Item.right)/2.0
			});
			graph.Connect(gl1,gl2);
		    }
		};
		process(IL->operator[](pos1).zero,0);
		process(IL->operator[](pos1).one,1);
	    }
	}
    }
    const auto connected=graph.connected_to(0);
    cerr<<equations.size()<<" equations"<<endl;
    cerr<<connected.size()<<" variables connected with the first one"<<endl;
    InexactEquationSolver<
	DifferentialMutations<AbsoluteMutations<>>
    > solver_hits(equations);
    auto init=make_shared<InitialDistributions>();
    ParamSet M;
    for(size_t i=0;i<totalN;i++){
	bool c=false;
	for(const size_t ii:connected)if(ii==i)c=true;
	if(c){
	    init<<make_shared<DistribGauss>(0,50);
	    M<<0.01;
	}else{
	    init<<make_shared<FixParam>(0);
	    M<<0;
	}
    }
    solver_hits.SetAbsoluteMutationCoefficients(M);
    solver_hits.SetAbsoluteMutationsProbability(0.1);
    solver_hits.SetThreadCount(thr_cnt);
    solver_hits.Init(connected.size()*7,init,engine);
    cerr<<"hits:"<<endl;
    cerr<<solver_hits.ParamCount()<<" variables"<<endl;
    cerr<<solver_hits.PopulationSize()<<" points"<<endl;
    SortedPoints<double> opt_min,opt_max;
    while(
	!solver_hits.AbsoluteOptimalityExitCondition(0.01)
    ){
	solver_hits.Iterate(engine);
	auto &min=solver_hits.Optimality(),
	    &max=solver_hits.Optimality(solver_hits.PopulationSize()-1);
	cerr<<solver_hits.iteration_count()<<" iterations; "
	<<min<<"<chi^2<"<<max<<" ; ";
	opt_min << point<double>(solver_hits.iteration_count(),min);
	opt_max << point<double>(solver_hits.iteration_count(),max);
	double d=0;
	for(const auto&p:solver_hits.ParametersStatistics())d+=p.uncertainty();
	d/=solver_hits.ParamCount();
	cerr<<"<dispersion>="<<d<<"             \r";
    }
    cerr<<endl;
    Plot<double>().Line(opt_min,"").Line(opt_max,"")
    <<"set log y"<<"set xlabel 'Iteration index'";
    Plotter::Instance()<<"unset log y";
    cerr<<"chi^2/D = "<<solver_hits.Optimality()/(equations.size()-solver_hits.ParamCount())<<endl;
    const auto&P=solver_hits.Parameters();
    hist<double> eq_left,eq_right,delta_hits;
    for(size_t i=0;i<totalN;i++)
	delta_hits<<point<value<double>>(double(i),P[i]);
    for(const auto&eq:equations){
	static size_t i=0;
	eq_left<<point<value<double>>(double(i),eq.left(P));
	eq_right<<point<value<double>>(double(i),eq.right);
	i++;
    }
    Plot<double>().Hist(eq_left,"left").Hist(eq_right,"right")<<"set key on"
    <<"set xlabel 'equation index'"<<"unset log y"<<"set yrange [-30:30]";
    Plot<double>().Hist(delta_hits)<<"set xlabel 'global slot index'"<<"set yrange [-30:30]";
    for(size_t i=0;i<totalN;i++){
	const StripPos slot=DeltaT->PositionOfGlobalNumber(i);
	const auto& ab=AB->operator[](slot).peak.val();
	auto&Delta=DeltaT->item(slot);
	Delta.A+=P[i]-(ab/2.0);
	Delta.B+=P[i]+(ab/2.0);
    }

    cout<<(*DeltaT);
}
