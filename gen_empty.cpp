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
	    auto&item=DeltaT->item({.layer=L,.slot=i});
	    item.A=0;item.B=0;
	    item.dA=0;item.dB=0;
	}
    }
    cout<<(*DeltaT);
}
