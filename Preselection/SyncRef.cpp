/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  @file TaskE.cpp
 */
#include <iostream>
#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <IO/gethist.h>
#include "SyncRef.h"
using namespace std;
TaskSyncRef::TaskSyncRef(const char * name, const char * description)
:JPetTask(name, description){}
auto const neighbour_delta_id_max=4;
void TaskSyncRef::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	int n_slots_in_half_layer = fBarrelMap.opositeDeltaID(*layer.second);
	for (size_t thr=1;thr<=4;thr++){
	    //ToDo:...
	}
    }
}
void TaskSyncRef::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	if (fHits.empty()) {
	    fHits.push_back(*currHit);
	} else {
	    if (fHits[0].getTimeWindowIndex() == currHit->getSignalB().getTimeWindowIndex()) {
		fHits.push_back(*currHit);
	    } else {
		fillCoincidenceHistos(fHits);
		fHits.clear();
		fHits.push_back(*currHit);
	    }
	}
    }
}
void TaskSyncRef::fillCoincidenceHistos(const vector<JPetHit>& hits){
    for (auto i = hits.begin(); i != hits.end(); ++i){
	for (auto j = i; ++j != hits.end(); ){
	    auto& hit1 = *i;
	    auto& hit2 = *j;
	    const auto layer1_n=fBarrelMap.getLayerNumber(hit1.getBarrelSlot().getLayer());
	    const auto layer2_n=fBarrelMap.getLayerNumber(hit2.getBarrelSlot().getLayer());
	    const auto slot1=fBarrelMap.getSlotNumber(hit1.getBarrelSlot());
	    const auto slot2=fBarrelMap.getSlotNumber(hit2.getBarrelSlot());
	    if ((layer1_n == layer2_n)&&(slot1!=slot2)) {
		map<int,double> lead_times_1_A = hit1.getSignalA().getRecoSignal().getRawSignal()
		.getTimesVsThresholdNumber(JPetSigCh::Leading);
		map<int,double> lead_times_2_A = hit2.getSignalA().getRecoSignal().getRawSignal()
		.getTimesVsThresholdNumber(JPetSigCh::Leading);
		map<int,double> lead_times_1_B = hit1.getSignalB().getRecoSignal().getRawSignal()
		.getTimesVsThresholdNumber(JPetSigCh::Leading);
		map<int,double> lead_times_2_B = hit2.getSignalB().getRecoSignal().getRawSignal()
		.getTimesVsThresholdNumber(JPetSigCh::Leading);
		for(int thr=1;thr<=1;thr++){//now we have data for only one threshold
		    if(
			(lead_times_1_A.count(thr)>0)&&(lead_times_1_B.count(thr)>0)&&
			(lead_times_2_A.count(thr)>0)&&(lead_times_2_B.count(thr)>0)
		    ){
			auto hit_1=(lead_times_1_A[thr]+lead_times_1_B[thr])/2000.0,
			hit_2=(lead_times_2_A[thr]+lead_times_2_B[thr])/2000.0,
			diff_1_2=hit_1-hit_2,
			diff_AB_1 =(lead_times_1_A[thr]-lead_times_1_B[thr])/1000.0,
			diff_AB_2 =(lead_times_2_A[thr]-lead_times_2_B[thr])/1000.0;
			if(fabs(diff_1_2)<200.0){
			    //ToDo:...
			}
		    }
		}
	    }
	}
    }
}
void TaskSyncRef::terminate(){}
void TaskSyncRef::setWriter(JPetWriter* writer){fWriter =writer;}
