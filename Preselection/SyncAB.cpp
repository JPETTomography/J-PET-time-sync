#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <j-pet-framework-extension/BarrelExtensions.h>
#include <j-pet-framework-extension/deltas.h>
#include <IO/gethist.h>
#include <Calc/convention.h>
#include "SyncAB.h"
using namespace std;
TaskSyncAB::TaskSyncAB(const char * name, const char * description)
:TOT_Hists(name, description){}
TaskSyncAB::~TaskSyncAB(){}
void TaskSyncAB::init(const JPetTaskInterface::Options&opts){
    TOT_Hists::init(opts);
    fSync=make_shared<Synchronization>(map(),cin,DefaultTimeCalculation);
    fCut=make_JPetMap<TOT_cut>();
    cin>>(*fCut);
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
	const auto tot=getTOTs(*currHit);
	const auto strip=map()->getStripPos(currHit->getBarrelSlot());
	bool passed=true;
	for(size_t thr=0;thr<4;thr++)
	    passed&=(tot.A[thr]>fCut->operator[](strip).A[thr])&&
		(tot.B[thr]>fCut->operator[](strip).B[thr]);
	if(passed){
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
}
void TaskSyncAB::fillCoincidenceHistos(){
    for (auto i = fHits.begin(); i != fHits.end(); ++i){
	for (auto j = i; ++j != fHits.end(); ){
	    const auto& hit1 = *i;
	    const auto& hit2 = *j;
	    const auto strip1=map()->getStripPos(hit1.getBarrelSlot());
	    const auto strip2=map()->getStripPos(hit2.getBarrelSlot());
	    if(strip1.layer == strip2.layer){
		const int delta_ID = map()->calcDeltaID(hit1.getBarrelSlot(), hit2.getBarrelSlot());
		if(delta_ID==(map()->getSlotsCount(strip1.layer)/2)){
		    const auto times1=fSync->GetTimes(hit1);
		    getStatistics()
			.getHisto1D(LayerSlotThr(strip1.layer,strip1.slot,1).c_str())
			.Fill(times1.A-times1.B);
		    fillTOTHistos(hit1,"coincidence");
		    const auto times2=fSync->GetTimes(hit2);
		    getStatistics()
			.getHisto1D(LayerSlotThr(strip2.layer,strip2.slot,1).c_str())
			.Fill(times2.A-times2.B);
		    fillTOTHistos(hit2,"coincidence");
		}
	    }
	}
    }
    fHits.clear();
}
void TaskSyncAB::terminate(){}
