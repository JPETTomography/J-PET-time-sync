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
#include "SyncAB.h"
using namespace std;
TaskSyncAB::TaskSyncAB(const char * name, const char * description)
:JPetTask(name, description){}
TaskSyncAB::~TaskSyncAB(){}
void TaskSyncAB::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	for (size_t thr=1;thr<=1;thr++){
	    for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
		auto histo_name = LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),sl,thr);
		char * histo_title = Form("%s;Delta_t", histo_name.c_str()); 
		getStatistics().createHistogram( new TH1F(histo_name.c_str(), histo_title,1200, -60.,+60.));
	    }
	}
    }
}
void TaskSyncAB::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	map<int,double> lead_times_A = currHit->getSignalA().getRecoSignal().getRawSignal()
	    .getTimesVsThresholdNumber(JPetSigCh::Leading);
	map<int,double> lead_times_B = currHit->getSignalB().getRecoSignal().getRawSignal()
	    .getTimesVsThresholdNumber(JPetSigCh::Leading);
	for(size_t thr=1;thr<=1;thr++){
	    if(
		(lead_times_A.count(thr)>0)&&(lead_times_B.count(thr)>0)
	    ){
		auto diff_AB=(lead_times_A[thr]-lead_times_B[thr])/1000.0;
		getStatistics().getHisto1D(
		    LayerSlotThr(
			fBarrelMap.getLayerNumber(currHit->getBarrelSlot().getLayer()),
			fBarrelMap.getSlotNumber(currHit->getBarrelSlot()),
			thr
		    ).c_str()
		).Fill(diff_AB);
	    }
	}
    }
}
void TaskSyncAB::terminate(){}
void TaskSyncAB::setWriter(JPetWriter* writer){fWriter =writer;}
