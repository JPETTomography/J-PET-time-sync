// this file is distributed under 
// MIT license
#include <map>
#include <JPetRawSignal/JPetRawSignal.h>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <JPetLargeBarrelExtensions/BarrelExtensions.h>
#include "TOT-conditions.h"
using namespace std;
const bool TOT_conditions(const JPetHit&hit){
    map<int,double> A = hit.getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber();
    map<int,double> B = hit.getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber();
    bool good=true;
    for(size_t thr=1;good&&(thr<=4);thr++)
	good&=(A[thr]>1.)&&(B[thr]>1.);
    return good;
}
