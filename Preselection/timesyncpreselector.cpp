#include <JPetWriter/JPetWriter.h>
#include <JPetUnpacker/Unpacker2/EventIII.h>
#include "timesyncpreselector.h"
using namespace std;
FrameworkTDCWrapper::FrameworkTDCWrapper(const char * name, const char * description, const TDCFUNC func)
:JPetTask(name, description),fFunction(func){}

void FrameworkTDCWrapper::exec(){  
	auto evt = reinterpret_cast<EventIII*> (getEvent());
	int ntdc = evt->GetTotalNTDCChannels();
	TClonesArray* tdcHits = evt->GetTDCChannelsArray();
	TDCChannel* tdcChannel;
	vector<pair<JPetSigCh,JPetSigCh>> param;
	for (int i = 0; i < ntdc; ++i) {
		tdcChannel = static_cast<TDCChannel*>(tdcHits->At(i));
		auto tomb_number =  tdcChannel->GetChannel();
		if (tomb_number % 65 == 0)continue;// skip trigger signals from TRB
		if( getParamBank().getTOMBChannels().count(tomb_number) == 0 ) {
			WARNING(Form("DAQ Channel %d appears in data but does not exist in the setup from DB.", tomb_number));
			continue;
		}
		JPetTOMBChannel& tomb_channel = getParamBank().getTOMBChannel(tomb_number);
		for(int j = 0; j < tdcChannel->GetHitsNum(); ++j){
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
			assert( tdcChannel->GetLeadTime(j) != -100000 );
			assert( tdcChannel->GetTrailTime(j) != -100000 );
			sigChTmpLead.setValue(tdcChannel->GetLeadTime(j) * 1000.);
			sigChTmpTrail.setValue(tdcChannel->GetTrailTime(j) * 1000.);
			param.push_back(make_pair(sigChTmpLead,sigChTmpTrail));
		}
	}
	fFunction(param);
}

void FrameworkTDCWrapper::terminate(){}
void FrameworkTDCWrapper::setParamManager(JPetParamManager* paramManager) {
	fParamManager = paramManager;
}
const JPetParamBank& FrameworkTDCWrapper::getParamBank() {
	assert(fParamManager);
	return fParamManager->getParamBank();
}


//void FrameworkWrapper::saveTimeWindow( JPetTimeWindow slot){
//	assert(fWriter);
//	fWriter->write(slot);
//}
//void FrameworkWrapper::setWriter(JPetWriter* writer) {
//	fWriter = writer;
//}
