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
#include <math_h/error.h>
using namespace std;
PrepareHits::PrepareHits(const char * name, const char * description,const std::shared_ptr<JPetMap<TOT_cut>>map)
:JPetTask(name, description),f_map(map){}
PrepareHits::~PrepareHits(){}
void PrepareHits::init(const JPetTaskInterface::Options& opts){
    fBarrelMap.buildMappings(getParamBank());
    for(auto & layer : getParamBank().getLayers()){
	auto l_n=fBarrelMap.getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
	    getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(l_n,sl,1)+"-A-after-cut").c_str(), "",500, 0.,100.));
	    getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(l_n,sl,1)+"-B-after-cut").c_str(), "",500, 0.,100.));
	}
    }
}
void PrepareHits::exec(){
    if(auto currSignal = dynamic_cast<const JPetRawSignal*const>(getEvent())){
	double TOT=currSignal->getTOTsVsThresholdNumber()[1]/1000.;
	const auto&bs=currSignal->getPM().getScin().getBarrelSlot();
	const auto layer=fBarrelMap.getLayerNumber(bs.getLayer());
	const auto slot=fBarrelMap.getSlotNumber(bs);
	const auto&item=f_map->Item(layer,slot);
	switch(currSignal->getPM().getSide()){
	    case JPetPM::SideA:
		TOT-=item.A;
	    break;
	    case JPetPM::SideB: 
		TOT-=item.B;
	    break;
	    default: 
		throw MathTemplates::Exception<PrepareHits>("signal has unknown side");
	}
	if(TOT>0){
	    switch(currSignal->getPM().getSide()){
		case JPetPM::SideA:
		    getStatistics().getHisto1D(("TOT-"+LayerSlotThr(layer,slot,1)+"-A-after-cut").c_str()).Fill(TOT+item.A);
		    break;
		case JPetPM::SideB: 
		    getStatistics().getHisto1D(("TOT-"+LayerSlotThr(layer,slot,1)+"-B-after-cut").c_str()).Fill(TOT+item.B);
		    break;
		default: 
		    throw MathTemplates::Exception<PrepareHits>("signal has unknown side");
	    }
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
