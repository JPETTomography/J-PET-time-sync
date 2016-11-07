#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <j-pet-framework-extension/BarrelExtensions.h>
#include <j-pet-framework-extension/deltas.h>
#include <IO/gethist.h>
#include "SyncAB.h"
using namespace std;
TaskSyncAB::TaskSyncAB(const char * name, const char * description)
:TOT_Hists(name, description){}
TaskSyncAB::~TaskSyncAB(){}
void TaskSyncAB::init(const JPetTaskInterface::Options&opts){
    TOT_Hists::init(opts);
    fSync=make_shared<Synchronization>(map(),cin,DefaultTimeCalculation);
    createTOTHistos("coincidence");
    for(auto & layer : getParamBank().getLayers()){
	const auto ln=map()->getLayerNumber(*layer.second);
	for(size_t sl=1,n=map()->getSlotsCount(ln);sl<=n;sl++){
	    getStatistics().createHistogram( new TH1F(LayerSlotThr(ln,sl,1).c_str(), "",1200, -60.,+60.));
	}
    }
}
void TaskSyncAB::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	if (fHits.empty()) {
	    fHits.push_back(*currHit);
	} else {
	    if (fHits[0].getTimeWindowIndex() == currHit->getTimeWindowIndex()) {
		fHits.push_back(*currHit);
	    } else {
		fillCoincidenceHistos();
		fHits.push_back(*currHit);
	    }
	}
    }
}
void TaskSyncAB::fillCoincidenceHistos(){
    for (const auto&hit1:fHits){
	const auto strip1=map()->getStripPos(hit1.getBarrelSlot());
	const auto times1=fSync->GetTimes(hit1);
	const double diff_AB_1 =(times1.A-times1.B);
	bool found=false;
	for (const auto&hit2:fHits){
	    const auto strip2=map()->getStripPos(hit2.getBarrelSlot());
	    if(strip1.layer == strip2.layer){
		const int delta_ID = map()->calcDeltaID(hit1.getBarrelSlot(), hit2.getBarrelSlot());
		auto opa_delta_ID=map()->getSlotsCount(strip1.layer)/2;
		if(delta_ID==opa_delta_ID){
		    found=true;
		    break;
		}
	    }
	}
	if(found){
	    getStatistics().getHisto1D(LayerSlotThr(strip1.layer,strip1.slot,1).c_str()).Fill(diff_AB_1);
	    fillTOTHistos(hit1,"coincidence");
	}
    }
    fHits.clear();
}
void TaskSyncAB::terminate(){}
