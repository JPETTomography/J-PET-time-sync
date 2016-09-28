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
    for(size_t layer=1;layer<=DeltaT->LayersCount();layer++){
	hist<double> A,B,eq_hist_left,eq_hist_right;
	cerr<<"LAYER "<<layer<<" : "<<endl;
	list<InexactEquation> equations;
	const size_t N=DeltaT->LayerSize(layer);
	for(size_t i=1;i<=N;i++){
	    const auto&AB_sync=AB->Item(layer,i);
	    const size_t A=i-1,B=A+N;
	    equations.push_back(in_eq( [A,B](const ParamSet&delta){return delta[B]-delta[A];} , {AB_sync.position.val(),AB_sync.width.val()} ));
	    const auto&neighbour_sync=Nei->Item(layer,i);
	    const size_t An=(A+3)%N,Bn=An+N;
	    value<double> leftpeak={neighbour_sync.position_left.val(),neighbour_sync.width_left.val()},
		rightpeak={neighbour_sync.position_right.val(),neighbour_sync.width_right.val()};
	    equations.push_back(in_eq([A,B,An,Bn](const ParamSet&delta){return (delta[Bn]+delta[An])-(delta[B]+delta[A]);}, leftpeak+rightpeak ));
	}
	for(size_t i=1;i<=(N/2);i++){
	    const auto&opo_sync=Opo->Item(layer,i);
	    const size_t A=i-1,B=A+N;
	    const size_t Ao=A+(N/2),Bo=Ao+N;
	    equations.push_back(in_eq([A,B,Ao,Bo](const ParamSet&delta){return ((delta[Bo]+delta[Ao])-(delta[B]+delta[A]))/2.0;}, {opo_sync.position.val(),opo_sync.width.val()}));
	}
	cerr<<equations.size()<<" equations"<<endl;
	InexactEquationSolver<DifferentialMutations<>> solver(equations);
	auto init=make_shared<GenerateByGauss>();
	while(init->Count()<(N*2))init<<make_pair(0,30);
	solver.Init(N*5,init,engine);
	cerr<<solver.ParamCount()<<" variables"<<endl;
	cerr<<solver.PopulationSize()<<" points"<<endl;
	while(!solver.AbsoluteOptimalityExitCondition(0.000001)){
	    solver.Iterate(engine);
	    cerr<<solver.iteration_count()<<" iterations; "
	    <<solver.Optimality()<<"<chi^2<"
	    <<solver.Optimality(solver.PopulationSize()-1)
	    <<"                 \r";
	}
	cout<<endl;
	cout<<"chi^2/D = "<<solver.Optimality()/(equations.size()-solver.ParamCount())<<endl;
	solver.SetUncertaintyCalcDeltas(parEq(solver.ParamCount(),0.01));
	for(size_t i=1;i<=N;i++){
	    A<<point<value<double>>(double(i),solver.ParametersWithUncertainties()[i-1]);
	    B<<point<value<double>>(double(i),solver.ParametersWithUncertainties()[i-1+N]);
	    DeltaT->Item(layer,i).A=solver.ParametersWithUncertainties()[i-1];
	    DeltaT->Item(layer,i).B=solver.ParametersWithUncertainties()[i-1+N];
	}
	{size_t index=0;
	    for(const auto&eq:equations){
		eq_hist_left<<point<value<double>>(double(index),eq.first(solver.Parameters()));
		eq_hist_right<<point<value<double>>(double(index),eq.second);
		index++;
	    }
	}
	Plot<double>().Hist(A,"A").Hist(B,"B")<<"set xlabel 'strip number'"<<"set ylabel 'time offset'"<<"set key on";
	Plot<double>().Hist(eq_hist_left,"left").Hist(eq_hist_right,"right")<<"set xlabel 'equation index'"<<"set ylabel 'value'"<<"set key on";
    }
    cout<<(*DeltaT);
}