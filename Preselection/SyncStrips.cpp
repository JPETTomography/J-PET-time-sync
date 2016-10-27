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
#include <j-pet-framework-extension/PetDict.h>
#include <j-pet-framework-extension/BarrelExtensions.h>
#include <j-pet-framework-extension/deltas.h>
#include <Calc/convention.h>
#include <IO/gethist.h>
#include "SyncStrips.h"
using namespace std;
TaskSyncStrips::TaskSyncStrips(const char * name, const char * description):JPetTask(name, description){}
void TaskSyncStrips::init(const JPetTaskInterface::Options& opts){
    fBarrelMap=make_shared<LargeBarrelMapping>(getParamBank());
    fSync=make_shared<Synchronization>(fBarrelMap,cin,DefaultTimeCalculation);
    f_AB_position=make_shared<JPetMap<SyncAB_results>>(fBarrelMap->getLayersSizes());
    cin>>(*f_AB_position);
    for(auto & layer : getParamBank().getLayers()){
	int n_slots_in_half_layer = fBarrelMap->getSlotsCount(*layer.second)/2;
	const auto layer_n=fBarrelMap->getLayerNumber(*layer.second);
	getStatistics().createHistogram( new TH1F(("DeltaID-for-coincidences-"+LayerThr(layer_n,1)).c_str(),"",n_slots_in_half_layer+2, -1.5, n_slots_in_half_layer+0.5));
	for(size_t slot=1;slot<=n_slots_in_half_layer;slot++){
	    string histo_name = "DeltaT-with-oposite-"+LayerSlotThr(layer_n,slot,1);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(),"",400, -100.,+100.));
	}
	for(size_t slot=1;slot<=fBarrelMap->getSlotsCount(*layer.second);slot++){
	    string histo_name = "DeltaT-with-neighbour-"+LayerSlotThr(layer_n,slot,1)+"-deltaid"+to_string(neighbour_delta_id);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(),"",400, -100.,+100.));
	}
    }
}
void TaskSyncStrips::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	bool accept=true;{
	    const auto strip=fBarrelMap->getStripPos(currHit->getBarrelSlot());
	    map<int,double> lead_times_A = currHit->getSignalA().getRecoSignal().getRawSignal().getTimesVsThresholdNumber(JPetSigCh::Leading);
	    map<int,double> lead_times_B = currHit->getSignalB().getRecoSignal().getRawSignal().getTimesVsThresholdNumber(JPetSigCh::Leading);
	    accept&=(lead_times_A.count(1)>0)&&(lead_times_B.count(1)>0);
	    if(accept){
		double diff_AB =(lead_times_A[1]-lead_times_B[1])/1000.0;
		accept&=f_AB_position->item(strip).peak.Contains(diff_AB);
	    }
	}
	if(accept){
	    if (fHits.empty()) {
		fHits.push_back(*currHit);
	    } else {
		if (fHits[0].getTimeWindowIndex() == currHit->getTimeWindowIndex()) {
		    fHits.push_back(*currHit);
		} else {
		    fillCoincidenceHistos();
		    fHits.push_back(*currHit);
		}
	    }
	}
    }
}
void TaskSyncStrips::fillCoincidenceHistos(){
    for (auto i = fHits.begin(); i != fHits.end(); ++i){
	for (auto j = i; ++j != fHits.end(); ){
	    auto& hit1 = *i;
	    auto& hit2 = *j;
	    const auto strip1=fBarrelMap->getStripPos(hit1.getBarrelSlot());
	    const auto strip2=fBarrelMap->getStripPos(hit2.getBarrelSlot());
	    if ((strip1.layer == strip2.layer)&&(strip1.slot!=strip2.slot)) {
		const auto layer=strip1.layer;
		const auto times1=fSync->GetTimes(hit1),times2=fSync->GetTimes(hit2);
		auto hit_1=(times1.A+times1.B)/2000.0,hit_2=(times2.A+times2.B)/2000.0,
		diff_1_2=hit_1-hit_2,
		diff_AB_1 =(times1.A-times1.B)/1000.0,
		diff_AB_2 =(times2.A-times2.B)/1000.0;
		if(fabs(diff_1_2)<200.0){
		    const int delta_ID = fBarrelMap->calcDeltaID(hit1.getBarrelSlot(), hit2.getBarrelSlot());
		    getStatistics().getHisto1D((
			"DeltaID-for-coincidences-"+LayerThr(layer,1)
		    ).c_str()).Fill(delta_ID);
		    auto opa_delta_ID=fBarrelMap->getSlotsCount(layer)/2;
		    if(delta_ID==opa_delta_ID){    
			if(strip1.slot<=opa_delta_ID)
			    getStatistics().getHisto1D(
				("DeltaT-with-oposite-"+
				    LayerSlotThr(layer,strip1.slot,1)
				).c_str()
			    ).Fill(diff_1_2);
			else
			    getStatistics().getHisto1D(
				("DeltaT-with-oposite-"+
				    LayerSlotThr(layer,strip2.slot,1)
				).c_str()
			    ).Fill(-diff_1_2);
		    }else{
			if(neighbour_delta_id==delta_ID){
			    if(
				(delta_ID==(strip2.slot-strip1.slot))
				||(delta_ID==((strip2.slot+fBarrelMap->getSlotsCount(layer))-strip1.slot))
			    )
				getStatistics().getHisto1D(
				    ("DeltaT-with-neighbour-"+
					LayerSlotThr(layer,strip1.slot,1)+
					"-deltaid"+to_string(neighbour_delta_id)
				    ).c_str()
				).Fill(diff_1_2);
			    else
				getStatistics().getHisto1D(
				    ("DeltaT-with-neighbour-"+
					LayerSlotThr(layer,strip2.slot,1)+
					"-deltaid"+to_string(neighbour_delta_id)
				    ).c_str()
				).Fill(-diff_1_2);
			}
		    }
		}
	    }
	}
    }
    fHits.clear();
}
void TaskSyncStrips::terminate(){}
void TaskSyncStrips::setWriter(JPetWriter* writer){fWriter =writer;}
