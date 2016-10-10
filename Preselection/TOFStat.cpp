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
 *  @file TaskD.cpp
 */

#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <IO/gethist.h>
#include <JPetWriter/JPetWriter.h>
#include "TOFStat.h"
using namespace std;
TOFStat::TOFStat(const char * name, const char * description):JPetTask(name, description){}
TOFStat::~TOFStat(){}
void TOFStat::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
	    string histo_name_A = "TOT_"+LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),sl,1)+"_A";
	    getStatistics().createHistogram( new TH1F(histo_name_A.c_str(), histo_name_A.c_str(),600, 0.,+60.));
	    string histo_name_B = "TOT_"+LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),sl,1)+"_B";
	    getStatistics().createHistogram( new TH1F(histo_name_B.c_str(), histo_name_B.c_str(),600, 0.,+60.));
	}
    }
}
void TOFStat::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	auto l=fBarrelMap.getLayerNumber(currHit->getBarrelSlot().getLayer()),s=fBarrelMap.getSlotNumber(currHit->getBarrelSlot());
	getStatistics().getHisto1D(
	    ("TOT_"+LayerSlotThr(l,s,1)+"_A").c_str()
	).Fill(currHit->getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber().at(1));
	getStatistics().getHisto1D(
	    ("TOT_"+LayerSlotThr(l,s,1)+"_B").c_str()
	).Fill(currHit->getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber().at(1));
    }
}
void TOFStat::terminate(){}
void TOFStat::setWriter(JPetWriter* writer) {fWriter =writer;}
