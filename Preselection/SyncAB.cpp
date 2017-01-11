// this file is distributed under 
// MIT license
#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <JPetLargeBarrelExtensions/BarrelExtensions.h>
#include <JPetLargeBarrelExtensions/TimeSyncDeltas.h>
#include <IO/gethist.h>
#include <Calc/convention.h>
#include "SyncAB.h"
#include "TOT-conditions.h"
using namespace std;
TaskSyncAB::TaskSyncAB(const char * name, const char * description)
:TOT_Hists(name, description){}
TaskSyncAB::~TaskSyncAB(){}
void TaskSyncAB::init(const JPetTaskInterface::Options&opts){
    TOT_Hists::init(opts);
    string thr_msg;cin>>thr_msg;list<size_t> lst;
    for(const char c:thr_msg)lst.push_back(c-'A');
    fSync=make_shared<Synchronization>(map(),cin,Thr(lst));
    createTOTHistos("coincidence");
    for(auto & layer : getParamBank().getLayers()){
	const auto ln=map()->getLayerNumber(*layer.second);
	for(size_t sl=1,n=map()->getSlotsCount(ln);sl<=n;sl++){
	    getStatistics().createHistogram( new TH1F(LayerSlot(ln,sl).c_str(), "",1200, -60.,+60.));
	}
    }
}
void TaskSyncAB::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	if(TOT_conditions(*currHit)){
	    const auto strip=map()->getStripPos(currHit->getBarrelSlot());
	    const auto times=fSync->get_times(*currHit);
	    getStatistics().getHisto1D(LayerSlot(strip).c_str()).Fill(times.A-times.B);
	    fillTOTHistos(*currHit,"coincidence");
	}
    }
}
void TaskSyncAB::terminate(){}
