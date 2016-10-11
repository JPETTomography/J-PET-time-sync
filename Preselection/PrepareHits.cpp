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
 *  @file TaskC.cpp
 */

#include <iostream>
#include <JPetWriter/JPetWriter.h>
#include "PrepareHits.h"
#include <IO/gethist.h>
using namespace std;
PrepareHits::PrepareHits(const char * name, const char * description,const std::shared_ptr<JPetMap<TOT_cut>>map)
:JPetTask(name, description),f_map(map){}
PrepareHits::~PrepareHits(){}
void PrepareHits::init(const JPetTaskInterface::Options& opts){
    for(auto & layer : getParamBank().getLayers()){
	auto l_n=fBarrelMap.getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
	    getStatistics().createHistogram( new TH1F(("TOT_"+LayerSlotThr(l_n,sl,1)+"_A").c_str(), "",500, 0.,100.));
	    getStatistics().createHistogram( new TH1F(("TOT_"+LayerSlotThr(l_n,sl,1)+"_B").c_str(), "",500, 0.,100.));
	}
    }
}
void PrepareHits::exec(){
    if(auto currSignal = dynamic_cast<const JPetRawSignal*const>(getEvent())){
	if (fSignals.empty()) {
	    fSignals.push_back(*currSignal);
	} else {
	    if (fSignals[0].getTimeWindowIndex() == currSignal->getTimeWindowIndex()) {
		fSignals.push_back(*currSignal);
	    } else {
		saveHits(createHits(fSignals));
		fSignals.clear();
		fSignals.push_back(*currSignal);
	    }
	}
    }
}

vector<JPetHit> PrepareHits::createHits(const vector<JPetRawSignal>& signals){
    vector<JPetHit> hits;
    for (auto i = signals.begin(); i != signals.end(); ++i) {
	for (auto j = i; ++j != signals.end(); ) {
	    if (i->getPM().getScin() == j->getPM().getScin()) {
		JPetRecoSignal recoSignalA;
		JPetRecoSignal recoSignalB;
		if (
		    (i->getPM().getSide() == JPetPM::SideA)
		    &&(j->getPM().getSide() == JPetPM::SideB)
		) {
		    recoSignalA.setRawSignal(*i);
		    recoSignalB.setRawSignal(*j);
		} else{ 
		    if( 
			(j->getPM().getSide() == JPetPM::SideA)
			&&(i->getPM().getSide() == JPetPM::SideB)
		    ){
			recoSignalA.setRawSignal(*j);
			recoSignalB.setRawSignal(*i);
		    } else {
			WARNING("TWO hits on the same scintillator side we ignore it");         
			continue;
		    }
		}
		const auto&bs=i->getPM().getScin().getBarrelSlot();
		const auto layer=fBarrelMap.getLayerNumber(bs.getLayer());
		const auto slot=fBarrelMap.getSlotNumber(bs);
		
		double TOT_A[4],TOT_B[4];
		for(size_t thr=1;thr<=4;thr++){
		    TOT_A[thr-1]=(recoSignalA.getRecoTimeAtThreshold(JPetSigCh::Trailing)-recoSignalA.getRecoTimeAtThreshold(JPetSigCh::Leading))/1000.;
		    TOT_B[thr-1]=(recoSignalB.getRecoTimeAtThreshold(JPetSigCh::Trailing)-recoSignalB.getRecoTimeAtThreshold(JPetSigCh::Leading))/1000.;
		}
		bool accepted=true;
		for(size_t thr=1;thr<4;thr++){
		    accepted&=(TOT_A[thr-1]>=TOT_A[thr])&&(TOT_B[thr-1]>=TOT_B[thr]);
		}
		accepted&=(TOT_A[0]>f_map->Item(layer,slot).A);
		accepted&=(TOT_B[0]>f_map->Item(layer,slot).B);
		if(accepted){
		    getStatistics().getHisto1D(("TOT_"+LayerSlotThr(layer,slot,1)+"_A").c_str()).Fill(TOT_A[0]);
		    getStatistics().getHisto1D(("TOT_"+LayerSlotThr(layer,slot,1)+"_B").c_str()).Fill(TOT_B[0]);
		    
		    JPetPhysSignal physSignalA;
		    JPetPhysSignal physSignalB;
		    physSignalA.setRecoSignal(recoSignalA);
		    physSignalB.setRecoSignal(recoSignalB);
		    JPetHit hit;
		    hit.setSignalA(physSignalA);
		    hit.setSignalB(physSignalB);
		    hit.setScintillator(i->getPM().getScin());
		    hit.setBarrelSlot(i->getPM().getScin().getBarrelSlot());
		    hits.push_back(hit);
		}
	    }
	}
    }
    return hits;
}
void PrepareHits::terminate(){
    saveHits(createHits(fSignals));
}
void PrepareHits::saveHits(const vector<JPetHit>&hits){
    assert(fWriter);
    for (auto hit : hits) 
	fWriter->write(hit);
}
void PrepareHits::setWriter(JPetWriter* writer) {fWriter =writer;}
