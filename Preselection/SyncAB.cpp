#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <j-pet-framework-extension/BarrelExtensions.h>
#include <j-pet-framework-extension/deltas.h>
#include <IO/gethist.h>
#include "SyncAB.h"
using namespace std;
TaskSyncAB::TaskSyncAB(const char * name, const char * description)
:JPetTask(name, description){}
TaskSyncAB::~TaskSyncAB(){}
void TaskSyncAB::init(const JPetTaskInterface::Options&){
    fBarrelMap=make_shared<LargeBarrelMapping>(getParamBank());
    fSync=make_shared<Synchronization>(fBarrelMap,cin,DefaultTimeCalculation);
    for(auto & layer : getParamBank().getLayers()){
	const auto ln=fBarrelMap->getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap->getSlotsCount(ln);sl<=n;sl++){
	    auto histo_name = LayerSlotThr(ln,sl,1);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(), "",1200, -60.,+60.));
	    for(size_t thr=1;thr<=4;thr++){
		getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(ln,sl,thr)+"-A-coincidence").c_str(), "",500, 0.,100.));
		getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(ln,sl,thr)+"-B-coincidence").c_str(), "",500, 0.,100.));
	    }
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
	const auto strip1=fBarrelMap->getStripPos(hit1.getBarrelSlot());
	const auto times1=fSync->GetTimes(hit1);
	const double diff_AB_1 =(times1.A-times1.B)/1000.0;
	bool found=false;
	for (const auto&hit2:fHits){
	    const auto strip2=fBarrelMap->getStripPos(hit2.getBarrelSlot());
	    if(strip1.layer == strip2.layer){
		const int delta_ID = fBarrelMap->calcDeltaID(hit1.getBarrelSlot(), hit2.getBarrelSlot());
		auto opa_delta_ID=fBarrelMap->getSlotsCount(strip1.layer)/2;
		if(delta_ID==opa_delta_ID){
		    found=true;
		    break;
		}
	    }
	}
	if(found){
	    const auto times=fSync->GetTimes(hit1);
	    getStatistics().getHisto1D(LayerSlotThr(strip1.layer,strip1.slot,1).c_str()).Fill((times.A-times.B)/1000.0);

	    auto TOTA=hit1.getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber(),
		TOTB=hit1.getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber();
	    for(size_t thr=1;thr<=4;thr++){
		getStatistics().getHisto1D(("TOT-"+LayerSlotThr(strip1.layer,strip1.slot,thr)+"-A-coincidence").c_str()).Fill(TOTA[thr]/1000.);
		getStatistics().getHisto1D(("TOT-"+LayerSlotThr(strip1.layer,strip1.slot,thr)+"-B-coincidence").c_str()).Fill(TOTB[thr]/1000.);
	    }
	}
    }
    fHits.clear();
}
void TaskSyncAB::terminate(){}
void TaskSyncAB::setWriter(JPetWriter* writer){fWriter =writer;}
