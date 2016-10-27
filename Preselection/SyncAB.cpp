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
 *  @file TaskE.cpp
 */
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
void TaskSyncAB::init(const JPetTaskInterface::Options& opts){
    fBarrelMap=make_shared<LargeBarrelMapping>(getParamBank());
    fSync=make_shared<Synchronization>(fBarrelMap,cin,DefaultTimeCalculation);
    for(auto & layer : getParamBank().getLayers()){
	const auto ln=fBarrelMap->getLayerNumber(*layer.second);
	for(size_t sl=1,n=fBarrelMap->getSlotsCount(ln);sl<=n;sl++){
	    auto histo_name = LayerSlotThr(ln,sl,1);
	    getStatistics().createHistogram( new TH1F(histo_name.c_str(), "",1200, -60.,+60.));
	}
    }
}
void TaskSyncAB::exec(){
    if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
	const auto times=fSync->GetTimes(*currHit);
	const auto diff_AB=(times.A-times.B)/1000.0;
	const auto strip=fBarrelMap->getStripPos(currHit->getBarrelSlot());
	getStatistics().getHisto1D(LayerSlotThr(strip.layer,strip.slot,1).c_str()).Fill(diff_AB);
    }
}
void TaskSyncAB::terminate(){}
void TaskSyncAB::setWriter(JPetWriter* writer){fWriter =writer;}
