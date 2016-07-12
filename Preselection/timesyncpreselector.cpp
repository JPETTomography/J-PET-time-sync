#include <TH1F.h>
#include <JPetWriter/JPetWriter.h>
#include <JPetUnpacker/Unpacker2/EventIII.h>
#include "timesyncpreselector.h"
using namespace std;
FrameworkTimeSyncPreselect::FrameworkTimeSyncPreselect(const char * name, const char * description)
:JPetTask(name, description),fWriter(nullptr),fHistHitsCount(make_shared<TH1F>("HitsCount","",10,-0.5,9.5)){}

void FrameworkTimeSyncPreselect::exec(){  
	auto evt = reinterpret_cast<EventIII*> (getEvent());
	int ntdc = evt->GetTotalNTDCChannels();
	TClonesArray* tdcHits = evt->GetTDCChannelsArray();
	TDCChannel* tdcChannel;
	for (int i = 0; i < ntdc; ++i) {
		tdcChannel = static_cast<TDCChannel*>(tdcHits->At(i));
		fHistHitsCount->Fill(tdcChannel->GetHitsNum());
// 		auto tomb_number =  tdcChannel->GetChannel();
// 		if (tomb_number % 65 == 0)continue;// skip trigger signals from TRB
// 		if( getParamBank().getTOMBChannels().count(tomb_number) == 0 ) {
// 			WARNING(Form("DAQ Channel %d appears in data but does not exist in the setup from DB.", tomb_number));
// 			continue;
// 		}
// 		JPetTOMBChannel& tomb_channel = getParamBank().getTOMBChannel(tomb_number);
// 		for(int j = 0; j < tdcChannel->GetHitsNum(); ++j){
// 			JPetSigCh sigChTmpLead, sigChTmpTrail;
// 			sigChTmpLead.setDAQch(tomb_number);
// 			sigChTmpTrail.setDAQch(tomb_number);
// 			sigChTmpLead.setType(JPetSigCh::Leading);
// 			sigChTmpTrail.setType(JPetSigCh::Trailing);
// 			sigChTmpLead.setThresholdNumber(tomb_channel.getLocalChannelNumber());
// 			sigChTmpTrail.setThresholdNumber(tomb_channel.getLocalChannelNumber());
// 			sigChTmpLead.setPM(tomb_channel.getPM());
// 			sigChTmpLead.setFEB(tomb_channel.getFEB());
// 			sigChTmpLead.setTRB(tomb_channel.getTRB());
// 			sigChTmpLead.setTOMBChannel(tomb_channel);
// 			sigChTmpTrail.setPM(tomb_channel.getPM());
// 			sigChTmpTrail.setFEB(tomb_channel.getFEB());
// 			sigChTmpTrail.setTRB(tomb_channel.getTRB());
// 			sigChTmpTrail.setTOMBChannel(tomb_channel);
// 			sigChTmpLead.setThreshold(tomb_channel.getThreshold());
// 			sigChTmpTrail.setThreshold(tomb_channel.getThreshold());
// 			assert( tdcChannel->GetLeadTime(j) != -100000 );
// 			assert( tdcChannel->GetTrailTime(j) != -100000 );
// 			sigChTmpLead.setValue(tdcChannel->GetLeadTime(j) * 1000.);
// 			sigChTmpTrail.setValue(tdcChannel->GetTrailTime(j) * 1000.);
// 		}
	}
}
void FrameworkTimeSyncPreselect::terminate(){
	assert(fWriter);
	fWriter->write(*fHistHitsCount);
}
void FrameworkTimeSyncPreselect::setParamManager(JPetParamManager* paramManager) {
	fParamManager = paramManager;
}
const JPetParamBank& FrameworkTimeSyncPreselect::getParamBank() {
	assert(fParamManager);
	return fParamManager->getParamBank();
}
void FrameworkTimeSyncPreselect::setWriter(JPetWriter* writer) {
	fWriter = writer;
}
