#include <iostream>
#include <Calc/convention.h>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <JPetLargeBarrelExtensions/TimeSyncDeltas.h>
using namespace std;
using namespace MathTemplates;
int main(int argc, char **argv) {
    auto DeltaT=make_JPetMap<SynchroStrip>();
    for(size_t L=1;L<=DeltaT->layersCount();L++){
	const size_t N=DeltaT->layerSize(L);
	for(size_t i=1;i<=N;i++){
	    DeltaT->item({.layer=L,.slot=i}).A=0;
	    DeltaT->item({.layer=L,.slot=i}).B=0;
	}
    }
    cout<<(*DeltaT);
}
