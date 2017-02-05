// this file is distributed under 
// MIT license
#include <iostream>
#include <map>
#include <math_h/error.h>
#include <JPetWriter/JPetWriter.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <JPetPhysSignal/JPetPhysSignal.h>
#include <Unpacker2/Unpacker2/EventIII.h>
#include <JPetLargeBarrelExtensions/BarrelExtensions.h>
#include <IO/gethist.h>
#include "PrepareHits.h"
#include <j-pet-config.h>
using namespace std;
PrepareHits::PrepareHits(const char * name, const char * description)
:TOT_Hists(name, description),fCurrEventNumber(0){}
PrepareHits::~PrepareHits(){}
void PrepareHits::init(const JPetTaskInterface::Options& opts){
    TOT_Hists::init(opts);
    createTOTHistos("hits");
}
void PrepareHits::exec(){
    if(auto evt = reinterpret_cast</*const*/ EventIII*const>(getEvent())){
	fCurrEventNumber++;
	int ntdc = evt->GetTotalNTDCChannels();
	JPetTimeWindow tslot;
	tslot.setIndex(fCurrEventNumber);
	TClonesArray* tdcHits = evt->GetTDCChannelsArray();
	for (int i = 0; i < ntdc; ++i) {
	    auto tdcChannel = dynamic_cast</*const*/ TDCChannel*const>(tdcHits->At(i));
	    auto tomb_number =  tdcChannel->GetChannel();
	    if (tomb_number % 65 == 0){
		continue;//skip trigger signals from TRB
	    }
	    if( getParamBank().getTOMBChannels().count(tomb_number) == 0 ) {
		WARNING(Form("DAQ Channel %d appears in data but does not exist in the setup from DB.", tomb_number));
		continue;
	    }
	    JPetTOMBChannel& tomb_channel = getParamBank().getTOMBChannel(tomb_number);
	    const int kNumHits = tdcChannel->GetHitsNum();
	    for(int j = 0; j < kNumHits; ++j){
		JPetSigCh sigChTmpLead,sigChTmpTrail;
		sigChTmpLead.setDAQch(tomb_number);
		sigChTmpTrail.setDAQch(tomb_number);
		sigChTmpLead.setType(JPetSigCh::Leading);
		sigChTmpTrail.setType(JPetSigCh::Trailing);
		sigChTmpLead.setThresholdNumber(tomb_channel.getLocalChannelNumber());
		sigChTmpTrail.setThresholdNumber(tomb_channel.getLocalChannelNumber());
		sigChTmpLead.setPM(tomb_channel.getPM());
		sigChTmpLead.setFEB(tomb_channel.getFEB());
		sigChTmpLead.setTRB(tomb_channel.getTRB());
		sigChTmpLead.setTOMBChannel(tomb_channel);
		sigChTmpTrail.setPM(tomb_channel.getPM());
		sigChTmpTrail.setFEB(tomb_channel.getFEB());
		sigChTmpTrail.setTRB(tomb_channel.getTRB());
		sigChTmpTrail.setTOMBChannel(tomb_channel);
		sigChTmpLead.setThreshold(tomb_channel.getThreshold());
		sigChTmpTrail.setThreshold(tomb_channel.getThreshold());
		if(tdcChannel->GetLeadTime (j)==-100000)continue;
		if(tdcChannel->GetTrailTime(j)==-100000)continue;
		sigChTmpLead .setValue(tdcChannel->GetLeadTime (j)*DAQ_2_TIME_UNIT);
		sigChTmpTrail.setValue(tdcChannel->GetTrailTime(j)*DAQ_2_TIME_UNIT);
		tslot.addCh(sigChTmpLead);
		tslot.addCh(sigChTmpTrail);
	    }
	}
	std::map<int,JPetSigCh> leadSigChs;
	std::map<int,JPetSigCh> trailSigChs;
	std::map<int, JPetRawSignal> signals; 
	const size_t nSigChs = tslot.getNumberOfSigCh();
	for (size_t i = 0; i < nSigChs; i++) {
	    JPetSigCh sigch = tslot.operator[](i);
	    int daq_channel = sigch.getDAQch();
	    if(sigch.getType()==JPetSigCh::Leading)
		leadSigChs[daq_channel]=sigch;
	    if(sigch.getType()==JPetSigCh::Trailing)
		trailSigChs[daq_channel]=sigch;
	}
	for (auto & chSigPair : leadSigChs){
	    int daq_channel = chSigPair.first;
	    if( trailSigChs.count(daq_channel) != 0 ){ 
		JPetSigCh & leadSigCh = chSigPair.second;
		JPetSigCh & trailSigCh = trailSigChs.at(daq_channel);
		double tot = trailSigCh.getValue() - leadSigCh.getValue();
		if( trailSigCh.getPM()!=leadSigCh.getPM() )
		    ERROR("Signals from same channel point to different PMTs! Check the setup mapping!!!");
		const auto&pm=leadSigCh.getPM();
		const auto layer=map()->getLayerNumber(pm.getBarrelSlot().getLayer());
		const auto slot=map()->getSlotNumber(pm.getBarrelSlot());
		const auto pmt_number = map()->calcGlobalPMTNumber(pm);
		double pmt_id = pm.getID();
		signals[pmt_id].addPoint( leadSigCh );
		signals[pmt_id].addPoint( trailSigCh );
	    }
	}
	for(auto & pmSignalPair : signals){
	    JPetRawSignal & signal = pmSignalPair.second;
	    signal.setTimeWindowIndex( tslot.getIndex() );
	    const JPetPM & pmt = getParamBank().getPM(pmSignalPair.first);
	    signal.setPM(pmt);
	    signal.setBarrelSlot(pmt.getBarrelSlot());
	    if (fSignals.empty()) {
		fSignals.push_back(signal);
	    } else {
		if (fSignals[0].getTimeWindowIndex() == signal.getTimeWindowIndex()) {
		    fSignals.push_back(signal);
		} else {
		    createAndStoreHits();
		    fSignals.push_back(signal);
		}
	    }
	}
    }
}
void PrepareHits::createAndStoreHits(){
    if(fSignals.empty())return;
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
		    writter().write(hit);
		    fillTOTHistos(hit,"hits");
		}
	    }
	}
    }
    fSignals.clear();
}
void PrepareHits::terminate(){}
