#include <JPetWriter/JPetWriter.h>
#include <JPetUnpacker/Unpacker2/EventIII.h>
#include "timesyncpreselector.h"

TaskA::TaskA(const char * name, const char * description):
  JPetTask(name, description),fCurrEventNumber(0){}


void TaskA::exec(){  
  // Get HLD Event
  auto evt = reinterpret_cast<EventIII*> (getEvent());
  //evt->GetTitle();
  //evt->GetName();

  // get number of TDC channels which fired in one TSlot
  int ntdc = evt->GetTotalNTDCChannels();

  JPetTimeWindow tslot;
  tslot.setIndex(fCurrEventNumber);

  // Get the array of TDC channels which fired
  TClonesArray* tdcHits = evt->GetTDCChannelsArray();

  // iterate over TDC channels
  TDCChannel* tdcChannel;
  for (int i = 0; i < ntdc; ++i) {

    tdcChannel = static_cast<TDCChannel*>(tdcHits->At(i));

    // get data channel number which corresponds to the TOMB Channel number
    auto tomb_number =  tdcChannel->GetChannel();

    if (tomb_number % 65 == 0) { // skip trigger signals from TRB
      continue;
    }

    if( getParamBank().getTOMBChannels().count(tomb_number) == 0 ) {
      WARNING(Form("DAQ Channel %d appears in data but does not exist in the setup from DB.", tomb_number));
      continue;
    }


    // get TOMBChannel object from database
    JPetTOMBChannel& tomb_channel = getParamBank().getTOMBChannel(tomb_number);
    
    // one TDC channel may record multiple signals in one TSlot
    // iterate over all signals from one TDC channel
    for(int j = 0; j < tdcChannel->GetHitsNum(); ++j){
      JPetSigCh sigChTmpLead, sigChTmpTrail;
      sigChTmpLead.setDAQch(tomb_number);
      sigChTmpTrail.setDAQch(tomb_number);
      
      sigChTmpLead.setType(JPetSigCh::Leading);
      sigChTmpTrail.setType(JPetSigCh::Trailing);

      sigChTmpLead.setThresholdNumber(tomb_channel.getLocalChannelNumber());
      sigChTmpTrail.setThresholdNumber(tomb_channel.getLocalChannelNumber());
      
      // store pointers to the related parametric objects
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
      
      // check for empty TDC times
      assert( tdcChannel->GetLeadTime(j) != -100000 );
      assert( tdcChannel->GetTrailTime(j) != -100000 );
      
      // finally, set the times in ps [raw times are in ns]
      sigChTmpLead.setValue(tdcChannel->GetLeadTime(j) * 1000.);
      sigChTmpTrail.setValue(tdcChannel->GetTrailTime(j) * 1000.);

      // and add the sigCh-s to TSlot
      tslot.addCh(sigChTmpLead);
      tslot.addCh(sigChTmpTrail);
    }
    
  }
  
  saveTimeWindow(tslot);
  
  fCurrEventNumber++;
}

void TaskA::terminate(){}
void TaskA::saveTimeWindow( JPetTimeWindow slot){
  assert(fWriter);
  fWriter->write(slot);
}
