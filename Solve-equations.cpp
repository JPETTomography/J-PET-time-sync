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
    
    auto AB=make_JPetMap<SyncAB_results>();
    {ifstream file;file.open(filenames[0]);if(file){file>>(*AB);file.close();}}
    auto Opo=make_half_JPetMap<SyncOposite_results>();
    {ifstream file;file.open(filenames[1]);if(file){file>>(*Opo);file.close();}}
    auto Nei=make_JPetMap<SyncNeighbour_results>();
    {ifstream file;file.open(filenames[2]);if(file){file>>(*Nei);file.close();}}
    auto DeltaT_D=make_JPetMap<DeltaT_results>();
    auto DeltaT=make_JPetMap<SynchroStrip>();
    Plotter::Instance().SetOutput(".","delta_t_sync");
    for(size_t L=1;L<=DeltaT->LayersCount();L++){
	cerr<<"=======LAYER "<<L<<" : "<<endl;
	list<InexactEquation> equations;
	const size_t N=DeltaT->LayerSize(L);
	for(size_t i=0;i<N;i++){
	    const auto&neighbour_sync=Nei->item({.layer=L,.slot=i+1});
	    equations.push_back(in_eq([i,N](const ParamSet&delta){return delta[(i+neighbour_delta_id)%N]-delta[i];}, (neighbour_sync.left+neighbour_sync.right)/2.0 ));
	}
	for(size_t i=0;i<(N/2);i++){
	    const auto&opo_sync=Opo->item({.layer=L,.slot=i+1});
	    equations.push_back(in_eq([i,N](const ParamSet&delta){return delta[i+(N/2)]-delta[i];}, opo_sync.peak ));
	}
	cerr<<equations.size()<<" equations"<<endl;
	cerr<<"hits:"<<endl;
	InexactEquationSolver<DifferentialMutations<>> solver_hits(equations);
	auto init=make_shared<GenerateUniform>();
	while(init->Count()<N)init<<make_pair(-100,100);
	solver_hits.SetThreadCount(thr_cnt);
	solver_hits.Init(N*10,init,engine);
	cerr<<solver_hits.ParamCount()<<" variables"<<endl;
	cerr<<solver_hits.PopulationSize()<<" points"<<endl;
	SortedPoints<double> opt_min,opt_max;
	while(!solver_hits.AbsoluteOptimalityExitCondition(0.001)){
	    solver_hits.Iterate(engine);
	    auto min=solver_hits.Optimality(),max=solver_hits.Optimality(solver_hits.PopulationSize()-1);
	    cerr<<solver_hits.iteration_count()<<" iterations; "
	    <<min<<"<chi^2<"<<max<<"                 \r";
	    opt_min << point<double>(solver_hits.iteration_count(),min);
	    opt_max << point<double>(solver_hits.iteration_count(),max);
	}
	Plot<double>().Line(opt_min,"").Line(opt_max,"")<<"set xlabel 'layer "+to_string(L)+"'";
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
	    eq_left<<point<value<double>>(double(i),eq.first(solver_hits.Parameters()));
	    eq_right<<point<value<double>>(double(i),eq.second);
	    i++;
	}
	Plot<double>().Hist(eq_left,"left").Hist(eq_right,"right")<<"set key on"<<"set xlabel 'layer "+to_string(L)+"'"<<"unset log y";
	Plot<double>().Hist(delta_hits,"HITS")<<"set key on"<<"set xlabel 'layer "+to_string(L)+"'";
	
	for(size_t i=1;i<=DeltaT->LayerSize(L);i++){
	    const auto& ab=AB->item({.layer=L,.slot=i});
	    const auto& delta=DeltaT_D->var_item({.layer=L,.slot=i})={.A=solution[i-1]-(ab.peak/2.0),.B=solution[i-1]+(ab.peak/2.0)};
	    DeltaT->var_item({.layer=L,.slot=i})={.A=delta.A.val(),.B=delta.B.val()};
	}
    }
    cout<<(*DeltaT);
    ofstream file("deltas_debug.txt");
    if(file){
	file<<(*DeltaT_D);
	file.close();
    }
}