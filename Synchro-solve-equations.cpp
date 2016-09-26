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
    for(size_t layer=1;layer<DeltaT->LayersCount();layer++){
	list<InexactEquation> equations;
	const size_t N=DeltaT->LayerSize(layer);
	for(size_t i=1;i<=N;i++){
	    //dtb-dta=AB
	    equations.push_back(in_eq([i,N](const ParamSet&P){return P[i-1 +N] - P[i-1];}, (AB->Item(layer,i).position) ));
	    //(dt2a+dt2b)-(dt1a+dt1b) = 2*NEI
	    equations.push_back(in_eq([i,N](const ParamSet&P){return (P[(i-1+3)%N +N] + P[(i-1+3)%N])-(P[i-1 +N] + P[i-1]);}, (Nei->Item(layer,i).position_left+Nei->Item(layer,i).position_right) ));
	}
	for(size_t i=1;i<=(N/2);i++){
	    //(dt2a+dt2b)-(dt1a+dt1b) = 2*OPO
	    equations.push_back(in_eq([i,N](const ParamSet&P){return (P[i-1+N/2 +N] + P[i-1+N/2])-(P[i-1 +N] + P[i-1]);}, (Opo->Item(layer,i).position*2.) ));
	}
	
	InexactEquationSolver<DifferentialMutations<>> solver(equations);
	auto init=make_shared<GenerateUniform>();
	while(init->Count()<(N*2))init<<make_pair(-100,100);
	solver.Init(N*10,init,engine);
	while(!solver.AbsoluteOptimalityExitCondition(0.00001)){
	    solver.Iterate(engine);
	    cerr<<solver.iteration_count()<<" iterations; "
	    <<solver.Optimality()<<"<S<"
	    <<solver.Optimality(solver.PopulationSize()-1)
	    <<"        \r";
	}
	for(size_t i=1;i<=N;i++){
	    DeltaT->Item(layer,i).A=solver[i-1];
	    DeltaT->Item(layer,i).B=solver[i-1+N];
	}
    }
    cout<<(*DeltaT);
}