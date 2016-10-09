#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <Genetic/equation2.h>
#include <Genetic/genetic.h>
#include <Genetic/initialconditions.h>
#include <IO/gethist.h>
#include <IO/PetDict.h>
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
    auto DeltaT=make_JPetMap<DeltaT_results>();
    Plotter::Instance().SetOutput(".","delta_t_sync");
    for(size_t layer=1;layer<DeltaT->LayersCount();layer++){
	cerr<<"=======LAYER "<<layer<<" : "<<endl;
	list<InexactEquation> equations;
	const size_t N=DeltaT->LayerSize(layer);
	for(size_t i=0;i<N;i++){
	    const auto&neighbour_sync=Nei->Item(layer,i+1);
	    equations.push_back(in_eq([i,N](const ParamSet&delta){return delta[(i+3)%N]-delta[i];}, (neighbour_sync.left+neighbour_sync.right)/2.0 ));
	}
	for(size_t i=0;i<(N/2);i++){
	    const auto&opo_sync=Opo->Item(layer,i+1);
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
	while(!solver_hits.AbsoluteOptimalityExitCondition(0.000001)){
	    solver_hits.Iterate(engine);
	    cerr<<solver_hits.iteration_count()<<" iterations; "
	    <<solver_hits.Optimality()<<"<chi^2<"
	    <<solver_hits.Optimality(solver_hits.PopulationSize()-1)
	    <<"                 \r";
	}
	cout<<endl;
	cout<<"chi^2/D = "<<solver_hits.Optimality()/(equations.size()-solver_hits.ParamCount())<<endl;
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
	Plot<double>().Hist(eq_left,"left").Hist(eq_right,"right")<<"set key on"<<"set xlabel 'layer "+to_string(layer)+"'";
	Plot<double>().Hist(delta_hits,"HITS")<<"set key on"<<"set xlabel 'layer "+to_string(layer)+"'";
	
	for(size_t i=1;i<DeltaT->LayerSize(layer);i++){
	    const auto& ab=AB->Item(layer,i);
	    DeltaT->Item(layer,i)={.A=solution[i-1]-(ab.peak/2.0),.B=solution[i-1]+(ab.peak/2.0)};
	}
    }
    cout<<(*DeltaT);
}