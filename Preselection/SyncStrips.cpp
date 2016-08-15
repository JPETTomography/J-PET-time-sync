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
#include "SyncStrips.h"
using namespace std;
TaskSyncStrips::TaskSyncStrips(const std::shared_ptr<JPetMap<SyncAB_results>> map,const char * name, const char * description)
:JPetTask(name, description),f_AB_position(map){}
auto const neighbour_delta_id=2;
void TaskSyncStrips::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	int n_slots_in_half_layer = fBarrelMap.opositeDeltaID(*layer.second);
	for (size_t thr=1;thr<=1;thr++){//Now we have data only for one threshold
	    {
		string histo_name = "Delta_ID_for_coincidences_"+LayerThr(fBarrelMap.getLayerNumber(*layer.second),thr);
		char * histo_title = Form("%s;#Delta ID", histo_name.c_str()); 
		getStatistics().createHistogram( new TH1F(histo_name.c_str(), histo_title,n_slots_in_half_layer+2, -1.5, n_slots_in_half_layer+0.5));
	    }
	    for(size_t slot=1;slot<=n_slots_in_half_layer;slot++){
		string histo_name = "Delta_t_with_oposite_"+LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),slot,thr);
		char * histo_title = Form("%s;#Delta ID", histo_name.c_str()); 
		getStatistics().createHistogram( new TH1F(histo_name.c_str(), histo_title,4000, -100.,+100.));
	    }
	    for(size_t slot=1;slot<=fBarrelMap.getNumberOfSlots(*layer.second);slot++){
		string histo_name = "Delta_t_with_neighbour_r_"+LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),slot,thr);
		char * histo_title = Form("%s;#Delta ID", histo_name.c_str()); 
		getStatistics().createHistogram( new TH1F(histo_name.c_str(), histo_title,4000, -100.,+100.));
	    }
	}
    }
}
void TaskSyncStrips::exec(){
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
		for(int thr=1;thr<=1;thr++){//now we have data for only one threshold
		    if(
			(lead_times_1_A.count(thr)>0)&&(lead_times_1_B.count(thr)>0)&&
			(lead_times_2_A.count(thr)>0)&&(lead_times_2_B.count(thr)>0)
		    ){
			auto diff_2_1_A=(lead_times_1_A[thr]-lead_times_2_A[thr])/1000.0;
			auto diff_AB_1 =(lead_times_1_A[thr]-lead_times_1_B[thr])/1000.0;
			auto diff_AB_2 =(lead_times_2_A[thr]-lead_times_2_B[thr])/1000.0;
			if(
			    (fabs(diff_2_1_A)<100.0)
			    &&(f_AB_position->operator()(layer1_n,slot1).Range().Contains(diff_AB_1))
			    &&(f_AB_position->operator()(layer2_n,slot2).Range().Contains(diff_AB_2))
			){
			    int delta_ID = fBarrelMap.calcDeltaID(hit1.getBarrelSlot(), hit2.getBarrelSlot());
			    getStatistics().getHisto1D((
				"Delta_ID_for_coincidences_"+LayerThr(layer1_n,thr)
			    ).c_str()).Fill(delta_ID);
			    auto opa_delta_ID=fBarrelMap.opositeDeltaID(hit1.getBarrelSlot().getLayer());
			    if(delta_ID==opa_delta_ID){    
				if(slot1<=opa_delta_ID)
				    getStatistics().getHisto1D(
					("Delta_t_with_oposite_"+LayerSlotThr(
					    layer1_n,slot1,thr   
					)).c_str()
				    ).Fill(diff_2_1_A);
				else
				    getStatistics().getHisto1D(
					("Delta_t_with_oposite_"+LayerSlotThr(
					    layer2_n,slot2,thr   
					)).c_str()
				    ).Fill(-diff_2_1_A);
			    }else{
				if(delta_ID==neighbour_delta_id){
				    if(
					((slot2-slot1)==neighbour_delta_id)||
					(((slot2+f_AB_position->LayerSize(layer2_n))-slot1)==neighbour_delta_id)
				    )
					getStatistics().getHisto1D(
					    ("Delta_t_with_neighbour_r_"+LayerSlotThr(
						layer1_n,slot1,thr
					    )).c_str()
					).Fill(diff_2_1_A);
				    else
					getStatistics().getHisto1D(
					    ("Delta_t_with_neighbour_r_"+LayerSlotThr(
						layer2_n,slot2,thr
					    )).c_str()
					).Fill(-diff_2_1_A);
				}
			    }
			}
		    }
		}
	    }
	}
    }
}
void TaskSyncStrips::terminate(){}
void TaskSyncStrips::setWriter(JPetWriter* writer){fWriter =writer;}
