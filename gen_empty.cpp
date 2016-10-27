#include <iostream>
#include <Calc/convention.h>
#include <j-pet-framework-extension/PetDict.h>
#include <j-pet-framework-extension/deltas.h>
using namespace std;
using namespace MathTemplates;
int main(int argc, char **argv) {
    auto DeltaT=make_JPetMap<SynchroStrip>();
    for(size_t L=1;L<=DeltaT->LayersCount();L++){
	const size_t N=DeltaT->LayerSize(L);
	for(size_t i=1;i<=N;i++){
	    DeltaT->var_item({.layer=L,.slot=i}).A=0;
	    DeltaT->var_item({.layer=L,.slot=i}).B=0;
	}
    }
    cout<<(*DeltaT);
}