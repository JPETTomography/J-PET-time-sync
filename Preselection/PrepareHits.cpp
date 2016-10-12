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
		    createHits(fSignals);
		    fSignals.clear();
		    fSignals.push_back(*currSignal);
		}
	    }
	}
    }
}

vector<JPetHit> PrepareHits::createHits(const vector<JPetRawSignal>& signals){
    assert(fWriter);
    for (size_t i=0,n=signals.size();i<n;i++) {
	for (auto j = i+1;j<n ;j++) {
	    if (signals[i].getPM().getScin() == signals[j].getPM().getScin()) {
		auto&scin=signals[i].getPM().getScin();
		JPetRecoSignal recoSignalA,recoSignalB;
		auto i_side=signals[i].getPM().getSide(),
		    j_side=signals[j].getPM().getSide();
		if ((i_side==JPetPM::SideA)&&(j_side==JPetPM::SideB)){
		    recoSignalA.setRawSignal(signals[i]);
		    recoSignalB.setRawSignal(signals[j]);
		}else{ 
		    if((j_side==JPetPM::SideA)&&(i_side==JPetPM::SideB)){
			recoSignalA.setRawSignal(signals[j]);
			recoSignalB.setRawSignal(signals[i]);
		    }else{
			WARNING("TWO hits on the same scintillator side we ignore it");         
			continue;
		    }
		}
		JPetPhysSignal physSignalA,physSignalB;
		physSignalA.setRecoSignal(recoSignalA);
		physSignalB.setRecoSignal(recoSignalB);
		JPetHit hit;
		hit.setSignalA(physSignalA);
		hit.setSignalB(physSignalB);
		hit.setScintillator(scin);
		hit.setBarrelSlot(scin.getBarrelSlot());
		fWriter->write(hit);
	    }
	}
    }
}
void PrepareHits::terminate(){createHits(fSignals);}
void PrepareHits::setWriter(JPetWriter* writer) {fWriter =writer;}
