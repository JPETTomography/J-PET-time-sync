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
    {ifstream file;file.open(filenames[2]);if(file){
	for(size_t i=0,n=neighbour_delta_id.size();i<n;i++)
	    file>>(*Nei[i]);file.close();
    }}
    const auto DeltaT_D=make_JPetMap<DeltaT_results>();
    const auto DeltaT=make_JPetMap<SynchroStrip>();
    cin>>(*DeltaT);
    Plotter::Instance().SetOutput(".","delta_t_sync");
    for(size_t L=1;L<=DeltaT->LayersCount();L++){
	cerr<<"=======LAYER "<<L<<" : "<<endl;
	list<InexactEquation> equations;
	ConnectionChecker slots(DeltaT->LayerSize(L));
	const size_t N=DeltaT->LayerSize(L);
	for(size_t i=0;i<N;i++){
	    const StripPos pos1={.layer=L,.slot=i+1};
	    if(AB->operator[](pos1).chi_sq>=0)
	    for(size_t ii=0,n=neighbour_delta_id.size();ii<n;ii++){
		const auto&neighbour_sync=Nei[ii]->operator[](pos1);
		auto i2=(i+neighbour_delta_id[ii])%N;
		const StripPos pos2={.layer=L,.slot=i2+1};
		if(//set of conditions for accepting the fit
		    //for neighbour coincidences spectrum
		    (AB->operator[](pos2).chi_sq>=0)&&
		    (neighbour_sync.chi_sq>=0.)&&
		    (neighbour_sync.assymetry<=4.0)&&
		    (neighbour_sync.assymetry>=0.25)&&
		    ((neighbour_sync.right-neighbour_sync.left).Below(7.0))&&
		    ((neighbour_sync.left+neighbour_sync.right).uncertainty()<2.5)
		){
		    equations.push_back({
			.left=[i,i2](const ParamSet&delta){return delta[i2]-delta[i];},
			.right=(neighbour_sync.left+neighbour_sync.right)/2.0
		    });
		    slots.Connect(i,i2);
		}
	    }
	}
	for(size_t i=0;i<(N/2);i++){
	    const auto i2=i+(N/2);
	    const auto&opo_sync=Opo->operator[]({.layer=L,.slot=i+1});
	    const StripPos pos1={.layer=L,.slot=i+1};
	    const StripPos pos2={.layer=L,.slot=i2+1};
	    if(
		(AB->operator[](pos1).chi_sq>=0)&&
		(AB->operator[](pos2).chi_sq>=0)&&
		(opo_sync.chi_sq>=0.)&&
		(opo_sync.peak.uncertainty()<2.5)
	    ){
		equations.push_back({
		    .left=[i,i2](const ParamSet&delta){return delta[i2]-delta[i];},
		    .right=opo_sync.peak
		});
		slots.Connect(i,i2);
	    }
	}
	equations.push_back({
	    .left=[](const ParamSet&delta){
		double res=0;
		for(const double&d:delta)
		    res+=pow(d,2);
		return res;
	    },
	    .right={0.0,double(equations.size())*50.0}
	});
	cerr<<equations.size()<<" equations"<<endl;
	auto connectedslots=slots.connected_to(0);
	if(connectedslots.size()<(N/2)){
	    //ToDo: check another area
	}
	cerr<<connectedslots.size()<<" connected"<<endl;
	InexactEquationSolver<DifferentialMutations<>> solver_hits(equations);
	auto init=make_shared<GenerateUniform>();
	for(size_t i=0;i<N;i++){
	    bool c=false;
	    for(size_t j=1;(!c)&&(j<connectedslots.size());j++)
		if(connectedslots[j]==i)c=true;
	    if(c)init<<make_pair(-100.,100.);
	    else init<<make_pair(-0.,0.);
	}
	solver_hits.SetThreadCount(thr_cnt);
	solver_hits.Init(N*15,init,engine);
	cerr<<"hits:"<<endl;
	cerr<<solver_hits.ParamCount()<<" variables"<<endl;
	cerr<<solver_hits.PopulationSize()<<" points"<<endl;
	SortedPoints<double> opt_min,opt_max;
	while(!solver_hits.AbsoluteOptimalityExitCondition(0.001)){
	    solver_hits.Iterate(engine);
	    auto min=solver_hits.Optimality(),
	    max=solver_hits.Optimality(solver_hits.PopulationSize()-1);
	    cerr<<solver_hits.iteration_count()<<" iterations; "
	    <<min<<"<chi^2<"<<max<<"                 \r";
	    opt_min << point<double>(solver_hits.iteration_count(),min);
	    opt_max << point<double>(solver_hits.iteration_count(),max);
	}
	Plot<double>().Line(opt_min,"").Line(opt_max,"")
	<<"set log y"<<"set xlabel 'layer "+to_string(L)+"'";
	Plotter::Instance()<<"unset log y";
	cerr<<endl;
	cerr<<"chi^2/D = "<<solver_hits.Optimality()/(equations.size()-solver_hits.ParamCount())<<endl;
	solver_hits.SetUncertaintyCalcDeltas(parEq(solver_hits.ParamCount(),0.01));
	const auto&solution=solver_hits.ParametersWithUncertainties();
	hist<double> eq_left,eq_right,delta_hits;
	for(size_t i=0;i<N;i++)
	    delta_hits<<point<value<double>>(double(i+1),solution[i]);
	for(const auto&eq:equations){
	    static size_t i=0;
	    eq_left<<point<value<double>>(double(i),eq.left(solver_hits.Parameters()));
	    eq_right<<point<value<double>>(double(i),eq.right);
	    i++;
	}
	Plot<double>().Hist(eq_left,"left").Hist(eq_right,"right")
	<<"set key on"<<"set xlabel 'layer "+to_string(L)+"'"<<"unset log y";
	Plot<double>().Hist(delta_hits,"HITS")
	<<"set key on"<<"set xlabel 'layer "+to_string(L)+"'";
	for(size_t i=1;i<=DeltaT->LayerSize(L);i++){
	    const StripPos slot={.layer=L,.slot=i};
	    const auto& ab=AB->operator[](slot);
	    const auto&delta=DeltaT_D->item(slot)
		={.A=solution[i-1]-(ab.peak/2.0),.B=solution[i-1]+(ab.peak/2.0)};
	    auto&Delta=DeltaT->item(slot);
	    Delta.A+=delta.A.val();
	    Delta.B+=delta.B.val();
	}
    }
    cout<<(*DeltaT);
    ofstream file("deltas_debug.txt");
    if(file){
	file<<(*DeltaT_D);
	file.close();
    }
}
