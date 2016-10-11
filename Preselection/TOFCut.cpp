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
#include "TOFCut.h"
using namespace std;
TOFCut::TOFCut(const char *name, const char *description,const shared_ptr<JPetMap<TOT_cut>>map)
:JPetTask(name,description),f_map(map){}
TOFCut::~TOFCut(){}
void TOFCut::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	auto l=fBarrelMap.getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
	    string histo_name_A = "TOT_"+LayerSlotThr(l,sl,1)+"_A";
	    getStatistics().createHistogram( new TH1F(("TOT_"+LayerSlotThr(l,sl,1)+"_A").c_str(), "",100, 0.,10.));
	    getStatistics().createHistogram( new TH1F(("TOT_"+LayerSlotThr(l,sl,1)+"_B").c_str(), "",100, 0.,10.));
	}
    }
}
void TOFCut::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	auto l=fBarrelMap.getLayerNumber(currHit->getBarrelSlot().getLayer()),s=fBarrelMap.getSlotNumber(currHit->getBarrelSlot());
	const auto& item=f_map->Item(l,s);
	auto TOT_A=currHit->getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber(),
	    TOT_B=currHit->getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber();
	const int thr=1;
	if(TOT_A[thr]>item.A)
	    if(TOT_B[thr]>item.B){
		fWriter->write(*currHit);
		getStatistics().getHisto1D(("TOT_"+LayerSlotThr(l,s,1)+"_A").c_str()).Fill(TOT_A[thr]);
		getStatistics().getHisto1D(("TOT_"+LayerSlotThr(l,s,1)+"_B").c_str()).Fill(TOT_B[thr]);
	    }
    }
}
void TOFCut::terminate(){}
void TOFCut::setWriter(JPetWriter* writer) {fWriter =writer;}
