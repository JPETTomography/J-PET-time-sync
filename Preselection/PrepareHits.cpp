#include <iostream>
#include <math_h/error.h>
#include <JPetWriter/JPetWriter.h>
#include <j-pet-framework-extension/BarrelExtensions.h>
#include <IO/gethist.h>
#include "PrepareHits.h"
using namespace std;
PrepareHits::PrepareHits(const char * name, const char * description):JPetTask(name, description){}
PrepareHits::~PrepareHits(){}
void PrepareHits::init(const JPetTaskInterface::Options& opts){
    fBarrelMap=make_shared<LargeBarrelMapping>(getParamBank());
    f_map=make_shared<JPetMap<TOT_cut>>(fBarrelMap->getLayersSizes());
    cin>>(*f_map);
    for(auto & layer : getParamBank().getLayers()){
	auto l_n=fBarrelMap->getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap->getSlotsCount(*layer.second);sl<=n;sl++){
	    for(size_t thr=1;thr<=4;thr++){
		getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(l_n,sl,thr)+"-A-after-cut").c_str(), "",500, 0.,100.));
		getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(l_n,sl,thr)+"-B-after-cut").c_str(), "",500, 0.,100.));
	    }
	}
    }
}
void PrepareHits::exec(){
    if(auto currSignal = dynamic_cast<const JPetRawSignal*const>(getEvent())){
	auto TOT=currSignal->getTOTsVsThresholdNumber();
	const auto&bs=currSignal->getPM().getScin().getBarrelSlot();
	const auto strippos=fBarrelMap->getStripPos(bs);
	const auto&item=f_map->item(strippos);
	bool passed=true;
	for(size_t thr=1;thr<4;thr++)passed&=(TOT[thr]>TOT[thr+1]);
	for(size_t thr=1;thr<=4;thr++)switch(currSignal->getPM().getSide()){
	    case JPetPM::SideA:    
		passed&=(TOT[thr]>item.A[thr-1]);
		break;
	    case JPetPM::SideB: 
		passed&=(TOT[thr]>item.B[thr-1]);
		break;
	    default: 
		throw MathTemplates::Exception<PrepareHits>("signal has unknown side");
	}
	if(passed){
	    for(size_t thr=1;thr<=4;thr++)switch(currSignal->getPM().getSide()){
		case JPetPM::SideA:    
		    getStatistics().getHisto1D(("TOT-"+LayerSlotThr(strippos.layer,strippos.slot,thr)+"-A-after-cut").c_str()).Fill(TOT[thr]);
		    break;
		case JPetPM::SideB: 
		    getStatistics().getHisto1D(("TOT-"+LayerSlotThr(strippos.layer,strippos.slot,thr)+"-B-after-cut").c_str()).Fill(TOT[thr]);
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
		    createAndStoreHits();
		    fSignals.push_back(*currSignal);
		}
	    }
	}
    }
}

void PrepareHits::createAndStoreHits(){
    if(fSignals.empty())return;
    assert(fWriter);
    for (size_t i=0,n=fSignals.size();i<n;i++) {
	auto&pm_i=fSignals[i].getPM();
	auto&scin=pm_i.getScin();
	for (auto j = i+1;j<n ;j++) {
	    auto&pm_j=fSignals[j].getPM();
	    if (pm_j.getScin()==scin) {
		const auto side_i=pm_i.getSide(),side_j=pm_j.getSide();
		if(side_i!=side_j){
		    JPetRecoSignal recoSignalA,recoSignalB;
		    recoSignalA.setRawSignal(fSignals[(side_i==JPetPM::SideA)?i:j]);
		    recoSignalB.setRawSignal(fSignals[(side_j==JPetPM::SideA)?i:j]);
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
    fSignals.clear();
}
void PrepareHits::terminate(){}
void PrepareHits::setWriter(JPetWriter* writer) {fWriter =writer;}
