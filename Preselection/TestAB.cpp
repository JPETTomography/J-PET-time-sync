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
#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <IO/gethist.h>
#include "TestAB.h"
using namespace std;
TestSyncAB::TestSyncAB(const char * name, const char * description)
:JPetTask(name, description){}
TestSyncAB::~TestSyncAB(){}
void TestSyncAB::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
	    auto histo_name = LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),sl,1);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(), "",1200, -60.,+60.));
	}
    }
}
void TestSyncAB::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	map<int,double> lead_times_A = currHit->getSignalA().getRecoSignal().getRawSignal()
	    .getTimesVsThresholdNumber(JPetSigCh::Leading);
	map<int,double> lead_times_B = currHit->getSignalB().getRecoSignal().getRawSignal()
	    .getTimesVsThresholdNumber(JPetSigCh::Leading);
	if((lead_times_A.count(1)>0)&&(lead_times_B.count(1)>0)){
	    vector<double> delta_AB={
		(lead_times_A[1]-lead_times_B[1])/1000.0,
		(lead_times_A[2]-lead_times_B[2])/1000.0,
		(lead_times_A[3]-lead_times_B[3])/1000.0,
		(lead_times_A[4]-lead_times_B[4])/1000.0
	    };
	    auto diff_AB=(delta_AB[0]+delta_AB[1])/2.0;
	    const auto layer=fBarrelMap.getLayerNumber(currHit->getBarrelSlot().getLayer());
	    const auto slot=fBarrelMap.getSlotNumber(currHit->getBarrelSlot());
	    getStatistics().getHisto1D(LayerSlotThr(layer,slot,1).c_str()).Fill(diff_AB);
	}
    }
}
void TestSyncAB::terminate(){}
void TestSyncAB::setWriter(JPetWriter* writer){fWriter =writer;}
