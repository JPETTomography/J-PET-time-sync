#include "deltas.h"
using namespace std;

Synchronization::Synchronization(const shared_ptr < AbstractBarrelMapping > map, istream & str, const Synchronization::TimeCalculation timecalc)
:f_mapping(map),f_time_calc(timecalc){
    f_offsets=make_shared<JPetMap<SynchroStrip>>(f_mapping->getLayersSizes());
    str>>(*f_offsets);
}
Synchronization::~Synchronization(){}
const SynchroStrip Synchronization::GetTimes(const JPetHit & hit) const{
    const auto& offsets=f_offsets->item(f_mapping->getStripPos(hit.getBarrelSlot()));
    map<int,double> lead_times_A = hit.getSignalA().getRecoSignal().getRawSignal().getTimesVsThresholdNumber(JPetSigCh::Leading);
    map<int,double> lead_times_B = hit.getSignalB().getRecoSignal().getRawSignal().getTimesVsThresholdNumber(JPetSigCh::Leading);
    vector<double> A,B;
    for(size_t thr=1;thr<=4;thr++){
	A.push_back(lead_times_A[thr]);
	B.push_back(lead_times_B[thr]);
    }
    return {.A=f_time_calc(A)+offsets.A,.B=f_time_calc(B)+offsets.B};
}
const double DefaultTimeCalculation(const vector< double >& P){
    return (P[0]+P[1]+P[2]+P[3])/4;
}
