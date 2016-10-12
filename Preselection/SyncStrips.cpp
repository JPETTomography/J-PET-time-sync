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
#include <Calc/convention.h>
#include <IO/gethist.h>
#include "SyncStrips.h"
using namespace std;
TaskSyncStrips::TaskSyncStrips(const std::shared_ptr<JPetMap<SyncAB_results>> map,const char * name, const char * description)
:JPetTask(name, description),f_AB_position(map){}
void TaskSyncStrips::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	int n_slots_in_half_layer = fBarrelMap.opositeDeltaID(*layer.second);
	const auto layer_n=fBarrelMap.getLayerNumber(*layer.second);
	getStatistics().createHistogram( new TH1F(("DeltaID-for-coincidences-"+LayerThr(layer_n,1)).c_str(),"",n_slots_in_half_layer+2, -1.5, n_slots_in_half_layer+0.5));
	for(size_t slot=1;slot<=n_slots_in_half_layer;slot++){
	    string histo_name = "DeltaT-with-oposite-"+LayerSlotThr(layer_n,slot,1);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(),"",400, -100.,+100.));
	}
	for(size_t slot=1;slot<=fBarrelMap.getNumberOfSlots(*layer.second);slot++){
	    string histo_name = "DeltaT-with-neighbour-"+LayerSlotThr(layer_n,slot,1)+"-deltaid"+to_string(neighbour_delta_id);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(),"",400, -100.,+100.));
	}
    }
}
void TaskSyncStrips::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	bool accept=true;{
	    const auto layer=fBarrelMap.getLayerNumber(currHit->getBarrelSlot().getLayer());
	    const auto slot=fBarrelMap.getSlotNumber(currHit->getBarrelSlot());
	    map<int,double> lead_times_A = currHit->getSignalA().getRecoSignal().getRawSignal().getTimesVsThresholdNumber(JPetSigCh::Leading);
	    map<int,double> lead_times_B = currHit->getSignalB().getRecoSignal().getRawSignal().getTimesVsThresholdNumber(JPetSigCh::Leading);
	    accept&=(lead_times_A.count(1)>0)&&(lead_times_B.count(1)>0);
	    if(accept){
		double diff_AB =(lead_times_A[1]-lead_times_B[1])/1000.0;
		accept&=f_AB_position->operator()(layer,slot).peak.Contains(diff_AB);
	    }
	}
	if(accept){
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
}
void TaskSyncStrips::fillCoincidenceHistos(const vector<JPetHit>& hits){
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
		auto hit_1=(lead_times_1_A[1]+lead_times_1_B[1])/2000.0,
		hit_2=(lead_times_2_A[1]+lead_times_2_B[1])/2000.0,
		diff_1_2=hit_1-hit_2,
		diff_AB_1 =(lead_times_1_A[1]-lead_times_1_B[1])/1000.0,
		diff_AB_2 =(lead_times_2_A[1]-lead_times_2_B[1])/1000.0;
		if(fabs(diff_1_2)<200.0){
		    int delta_ID = fBarrelMap.calcDeltaID(hit1.getBarrelSlot(), hit2.getBarrelSlot());
		    getStatistics().getHisto1D((
			"DeltaID-for-coincidences-"+LayerThr(layer1_n,1)
		    ).c_str()).Fill(delta_ID);
		    auto opa_delta_ID=fBarrelMap.opositeDeltaID(hit1.getBarrelSlot().getLayer());
		    if(delta_ID==opa_delta_ID){    
			if(slot1<=opa_delta_ID)
			    getStatistics().getHisto1D(
				("DeltaT-with-oposite-"+
				    LayerSlotThr(layer1_n,slot1,1)
				).c_str()
			    ).Fill(diff_1_2);
			else
			    getStatistics().getHisto1D(
				("DeltaT-with-oposite-"+
				    LayerSlotThr(layer2_n,slot2,1)
				).c_str()
			    ).Fill(-diff_1_2);
		    }else{
			if(neighbour_delta_id==delta_ID){
			    if(
				((slot2-slot1)==delta_ID)||(((slot2+f_AB_position->LayerSize(layer2_n))-slot1)==delta_ID)
			    )
				getStatistics().getHisto1D(
				    ("DeltaT-with-neighbour-"+
					LayerSlotThr(layer1_n,slot1,1)+
					"-deltaid"+to_string(neighbour_delta_id)
				    ).c_str()
				).Fill(diff_1_2);
			    else
				getStatistics().getHisto1D(
				    ("DeltaT-with-neighbour-"+
					LayerSlotThr(layer2_n,slot2,1)+
					"-deltaid"+to_string(neighbour_delta_id)
				    ).c_str()
				).Fill(-diff_1_2);
			}
		    }
		}
	    }
	}
    }
}
void TaskSyncStrips::terminate(){}
void TaskSyncStrips::setWriter(JPetWriter* writer){fWriter =writer;}
