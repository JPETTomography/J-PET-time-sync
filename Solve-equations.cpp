#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <Genetic/equation2.h>
#include <Genetic/genetic.h>
#include <Genetic/initialconditions.h>
#include <IO/gethist.h>
#include <j-pet-framework-extension/PetDict.h>
#include <j-pet-framework-extension/deltas.h>
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


    list<InexactEquation> equations;
    const auto totalN=DeltaT->SlotsCount();
    ConnectionChecker slots(totalN);

    for(size_t L=1;L<=DeltaT->LayersCount();L++){
	const size_t N=DeltaT->LayerSize(L);
	
	for(size_t i1=0;i1<N;i1++){
	    const StripPos pos1={.layer=L,.slot=i1+1};
	    const auto gl1=DeltaT->GlobalSlotNumber(pos1);
	    if(AB->operator[](pos1).chi_sq>=0)
	    for(size_t di=0;di<neighbour_delta_id.size();di++){
		const auto&Item=Nei[di]->operator[](pos1);
		const auto i2=(i1+neighbour_delta_id[di])%N;
		const StripPos pos2={.layer=L,.slot=i2+1};
		const auto gl2=DeltaT->GlobalSlotNumber(pos2);
		if(
		    (AB->operator[](pos2).chi_sq>=0)&&
		    (Item.chi_sq>=0.)&&
		    (Item.assymetry<=4.0)&&(Item.assymetry>=0.25)&&
		    ((Item.right-Item.left).Below(7.0))&&
		    ((Item.right-Item.left).Above(1.0))
		){
		    equations.push_back({
			.left=[gl1,gl2](const ParamSet&delta){return delta[gl2]-delta[gl1];},
			.right=(Item.left+Item.right)/2.0
		    });
		    slots.Connect(gl1,gl2);
		}
	    }
	}
	
	for(size_t i1=0;i1<(N/2);i1++){
	    const auto i2=i1+(N/2);
	    const auto&Item=Opo->operator[]({.layer=L,.slot=i1+1});
	    const StripPos pos1={.layer=L,.slot=i1+1};
	    const StripPos pos2={.layer=L,.slot=i2+1};
	    const auto gl1=DeltaT->GlobalSlotNumber(pos1);
	    const auto gl2=DeltaT->GlobalSlotNumber(pos2);
	    if(
		(AB->operator[](pos1).chi_sq>=0)&&
		(AB->operator[](pos2).chi_sq>=0)&&
		(Item.chi_sq>=0.)
	    ){
		equations.push_back({
		    .left=[gl1,gl2](const ParamSet&delta){return delta[gl2]-delta[gl1];},
		    .right=Item.peak
		});
		slots.Connect(gl1,gl2);
	    }
	}
	if(L<=IL->LayersCount()){
	    for(size_t i1=0;i1<N;i1++){
		const StripPos pos1={.layer=L,.slot=i1+1};
		auto process=[&pos1,&i1,&L,&N,&AB,&DeltaT,&slots,&equations](const SyncScatter_results&Item,const size_t coinc_index){
		    const auto&item=SyncLayerIndices[L-1][coinc_index];
		    const StripPos pos2={.layer=L+1,.slot=((i1*item.coef+item.offs)%DeltaT->LayerSize(L+1))+1};
		    const auto gl1=DeltaT->GlobalSlotNumber(pos1);
		    const auto gl2=DeltaT->GlobalSlotNumber(pos2);
		    if(
			(AB->operator[](pos1).chi_sq>=0)&&
			(AB->operator[](pos2).chi_sq>=0)&&
			(Item.chi_sq>=0.)&&
			(Item.assymetry<=10.0)&&(Item.assymetry>=0.1)&&
			((Item.right-Item.left).Below(7.0))&&
			((Item.right-Item.left).Above(1.0))
		    ){
			equations.push_back({
			    .left=[gl1,gl2](const ParamSet&delta){return delta[gl2]-delta[gl1];},
			    .right=(Item.left+Item.right)/2.0
			});
			slots.Connect(gl1,gl2);
		    }
		};
		process(IL->operator[](pos1).zero,0);
		process(IL->operator[](pos1).one,1);
	    }
	}
    }
    equations.push_back({
	.left=[](const ParamSet&delta){
	    double res=0;
	    for(const double&d:delta)
		res+=pow(d,2);
	    return res;
	},
	.right={0.0,10000.0*totalN}
    });
    cerr<<equations.size()<<" equations"<<endl;

    //ToDo: Take into account case when slot 1:1 isn't connected
    auto connectedslots=slots.connected_to(0);
    cerr<<connectedslots.size()<<" connected"<<endl;

    InexactEquationSolver<DifferentialMutations<>> solver_hits(equations);
    auto init=make_shared<InitialDistributions>();
    ParamSet deltas;
    for(size_t i=0;i<totalN;i++){
	bool c=false;
	for(size_t j=1;(!c)&&(j<connectedslots.size());j++)
	    if(connectedslots[j]==i)c=true;
	if(c)init<<make_shared<DistribUniform>(-100.,100.);
	else init<<make_shared<FixParam>(0);
	deltas<<0.01;
    }
    solver_hits.SetThreadCount(thr_cnt);
    solver_hits.Init(connectedslots.size()*5,init,engine);
    cerr<<"hits:"<<endl;
    cerr<<solver_hits.ParamCount()<<" variables"<<endl;
    cerr<<solver_hits.PopulationSize()<<" points"<<endl;
    SortedPoints<double> opt_min,opt_max;
    while(
	!solver_hits.ParametersDispersionExitCondition(deltas)
	&&
	!solver_hits.RelativeOptimalityExitCondition(0.0001)
    ){
	solver_hits.Iterate(engine);
	auto &min=solver_hits.Optimality(),
	    &max=solver_hits.Optimality(solver_hits.PopulationSize()-1);
	cerr<<solver_hits.iteration_count()<<" iterations; "
	<<min<<"<chi^2<"<<max<<"                 \r";
	opt_min << point<double>(solver_hits.iteration_count(),min);
	opt_max << point<double>(solver_hits.iteration_count(),max);
    }
    Plot<double>().Line(opt_min,"").Line(opt_max,"")
    <<"set log y"<<"set xlabel 'Iteration index'";
    Plotter::Instance()<<"unset log y";
    cerr<<endl;
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
	Delta.A+=P[i-1]-(ab/2.0);
	Delta.B+=P[i-1]+(ab/2.0);
    }

    cout<<(*DeltaT);
}
