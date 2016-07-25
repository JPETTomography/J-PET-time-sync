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
#include <iostream>
#include <JPetWriter/JPetWriter.h>
#include <JPetHitUtils/JPetHitUtils.h>
#include <IO/gethist.h>
#include "SyncAB.h"
using namespace std;
TaskSyncAB::TaskSyncAB(const char * name, const char * description)
:JPetTask(name, description){}
TaskSyncAB::~TaskSyncAB(){}
void TaskSyncAB::init(const JPetTaskInterface::Options& opts){
	fBarrelMap.buildMappings(getParamBank());
	for(auto & layer : getParamBank().getLayers()){
		for (size_t thr=1;thr<=4;thr++){
			for(size_t sl=0,n=fBarrelMap.getNumberOfSlots(*layer.second);sl<n;sl++){
				auto histo_name = LayerSlotThr(fBarrelMap.getLayerNumber(*layer.second),sl+1,thr);
				char * histo_title = Form("%s;Delta_t", histo_name.c_str()); 
				getStatistics().createHistogram( new TH1F(histo_name.c_str(), histo_title,200, -100., 100.));
			}
		}
	}
}
void TaskSyncAB::exec(){
	if(auto currHit = dynamic_cast<const JPetHit*const>(getEvent())){
		for(size_t thr=1;thr<=4;thr++){
			getStatistics().getHisto1D(
				LayerSlotThr(
					fBarrelMap.getLayerNumber(currHit->getBarrelSlot().getLayer()),
					fBarrelMap.getSlotNumber(currHit->getBarrelSlot()),
					thr
				).c_str()
			).Fill(JPetHitUtils::getTimeDiffAtThr(*currHit,thr));
		}
	}
}
void TaskSyncAB::terminate(){}
void TaskSyncAB::setWriter(JPetWriter* writer){fWriter =writer;}
