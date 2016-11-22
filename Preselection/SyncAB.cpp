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
	const auto strip=map()->getStripPos(currHit->getBarrelSlot());
	const auto times=fSync->GetTimes(*currHit);
	getStatistics()
	    .getHisto1D(LayerSlot(strip.layer,strip.slot).c_str())
	    .Fill(times.A-times.B);
	fillTOTHistos(*currHit,"coincidence");
    }
}
void TaskSyncAB::terminate(){}
