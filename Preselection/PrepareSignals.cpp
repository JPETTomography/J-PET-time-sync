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
 *  @file TaskA.cpp
 */

#include <JPetUnpacker/Unpacker2/EventIII.h>
#include <JPetWriter/JPetWriter.h>
#include "PrepareSignals.h"
#include <IO/gethist.h>
using namespace std;
PrepareSignals::PrepareSignals(const char * name, const char * description)
:JPetTask(name, description),fCurrEventNumber(0){}
PrepareSignals::~PrepareSignals(){}
void PrepareSignals::init(const JPetTaskInterface::Options& opts){
    for(auto & layer : getParamBank().getLayers()){
	auto l=fBarrelMap.getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<=n;sl++){
	    string histo_name_A = "TOT_"+LayerSlotThr(l,sl,1)+"_A";
	    getStatistics().createHistogram( new TH1F(("TOT_"+LayerSlotThr(l,sl,1)+"_A").c_str(), "",500, 0.,100.));
	    getStatistics().createHistogram( new TH1F(("TOT_"+LayerSlotThr(l,sl,1)+"_B").c_str(), "",500, 0.,100.));
	}
    }
}
void PrepareSignals::exec(){
    if(auto evt = reinterpret_cast</*const*/ EventIII*const>(getEvent())){
	int ntdc = evt->GetTotalNTDCChannels();
	JPetTimeWindow tslot;
	tslot.setIndex(fCurrEventNumber);
	TClonesArray* tdcHits = evt->GetTDCChannelsArray();
	for (int i = 0; i < ntdc; ++i) {
	    auto tdcChannel = dynamic_cast</*const*/ TDCChannel*const>(tdcHits->At(i));
	    auto tomb_number =  tdcChannel->GetChannel();
	    if (tomb_number % 65 == 0)continue;//skip trigger signals from TRB
	    assert(0!=getParamBank().getTOMBChannels().count(tomb_number));
	    JPetTOMBChannel& tomb_channel = getParamBank().getTOMBChannel(tomb_number);
	    for(int j = tdcChannel->GetHitsNum()-1; j < tdcChannel->GetHitsNum(); ++j){
		JPetSigCh sigChTmpLead, sigChTmpTrail;
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
		if((tdcChannel->GetLeadTime (j)+0.1)>tdcChannel->GetTrailTime(j))continue;
		sigChTmpLead .setValue(tdcChannel->GetLeadTime (j)*1000.);
		sigChTmpTrail.setValue(tdcChannel->GetTrailTime(j)*1000.);
		tslot.addCh(sigChTmpLead);
		tslot.addCh(sigChTmpTrail);
	    }
	}
	fCurrEventNumber++;
	{
	    map<int,JPetSigCh> leadSigChs;
	    map<int,JPetSigCh> trailSigChs;
	    map<int, JPetRawSignal> signals; 
	    const size_t nSigChs = tslot.getNumberOfSigCh();
	    for (size_t i = 0; i < nSigChs; i++) {
		JPetSigCh sigch = tslot.operator[](i);
		int daq_channel = sigch.getDAQch();
		if( sigch.getType() == JPetSigCh::Leading ){
		    leadSigChs[ daq_channel ] = sigch;
		}
		if( sigch.getType() == JPetSigCh::Trailing ){
		    trailSigChs[ daq_channel ] = sigch;
		}
	    }
	    for (auto & chSigPair : leadSigChs) {
		int daq_channel = chSigPair.first;
		if( trailSigChs.count(daq_channel) != 0 ){ 
		    JPetSigCh & leadSigCh = chSigPair.second;
		    JPetSigCh & trailSigCh = trailSigChs.at(daq_channel);
		    double tot = trailSigCh.getValue() - leadSigCh.getValue();
		    if( trailSigCh.getPM()!=leadSigCh.getPM() )
			ERROR("Signals from same channel point to different PMTs! Check the setup mapping!!!");
		    const auto&pm=leadSigCh.getPM();
		    const auto layer=fBarrelMap.getLayerNumber(pm.getBarrelSlot().getLayer());
		    const auto slot=fBarrelMap.getSlotNumber(pm.getBarrelSlot());
		    const auto pmt_number = fBarrelMap.calcGlobalPMTNumber(pm);
		    double pmt_id = pm.getID();
		    signals[pmt_id].addPoint( leadSigCh );
		    signals[pmt_id].addPoint( trailSigCh );
		    if(leadSigCh.getThresholdNumber()==1)
			switch(pm.getSide()){
			    case JPetPM::SideA: getStatistics().getHisto1D(("TOT_"+LayerSlotThr(layer,slot,1)+"_A").c_str()).Fill(tot / 1000.);
			    case JPetPM::SideB: getStatistics().getHisto1D(("TOT_"+LayerSlotThr(layer,slot,1)+"_B").c_str()).Fill(tot / 1000.);
			}
		}
	    }
	    for (auto & chSigPair : trailSigChs) {
		int daq_channel = chSigPair.first;
		int pmt_number = fBarrelMap.calcGlobalPMTNumber(chSigPair.second.getPM());
	    }    
	    for(auto & pmSignalPair : signals){
		JPetRawSignal & signal = pmSignalPair.second;
		signal.setTimeWindowIndex( tslot.getIndex() );
		const JPetPM & pmt = getParamBank().getPM(pmSignalPair.first);
		signal.setPM(pmt);
		signal.setBarrelSlot(pmt.getBarrelSlot());
		fWriter->write(signal);
	    }
	}
	
    }
}
void PrepareSignals::terminate(){}
void PrepareSignals::setWriter(JPetWriter* writer) {
    fWriter = writer;
}
void PrepareSignals::setParamManager(JPetParamManager* paramManager) {
    fParamManager = paramManager;
}
const JPetParamBank& PrepareSignals::getParamBank()const {
    assert(fParamManager);
    return fParamManager->getParamBank();
}