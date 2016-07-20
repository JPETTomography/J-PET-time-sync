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
#include "SyncStrips.h"
using namespace std;
TaskSyncStrips::TaskSyncStrips(const char * name, const char * description)
:JPetTask(name, description){}

void TaskSyncStrips::init(const JPetTaskInterface::Options& opts){
	fBarrelMap.buildMappings(getParamBank());
	for(auto & layer : getParamBank().getLayers()){
		for (int thr=1;thr<=4;thr++){
			string histo_name = "Delta_ID_for_coincidences_"+LayerThr(fBarrelMap.getLayerNumber(*layer.second),thr);
			char * histo_title = Form("%s;#Delta ID", histo_name.c_str()); 
			int n_slots_in_half_layer = fBarrelMap.getNumberOfSlots(*layer.second) / 2;
			getStatistics().createHistogram( new TH1F(histo_name.c_str(), histo_title,n_slots_in_half_layer+2, -1.5, n_slots_in_half_layer+0.5));
		}
	}
}
void TaskSyncStrips::exec(){
	auto currHit = dynamic_cast<JPetHit*>(getEvent());
	if(currHit){
		if (fHits.empty()) {
			fHits.push_back(*currHit);
		} else {
			if (fHits[0].getTimeWindowIndex() == currHit->getSignalB().getTimeWindowIndex()) {
				fHits.push_back(*currHit);
			} else {
				fillCoincidenceHistos(fHits);
				fHits.clear();
				fHits.push_back(*currHit);
			}
		}
	}
}
void TaskSyncStrips::fillCoincidenceHistos(std::vector<JPetHit>& hits){
	//ToDo: use const modifier to transfer this parameter
	for (auto i = hits.begin(); i != hits.end(); ++i){
		for (auto j = i; ++j != hits.end(); ){
			JPetHit & hit1 = *i;
			JPetHit & hit2 = *j;
			if (
				(hit1.getBarrelSlot().getLayer() == hit2.getBarrelSlot().getLayer())
				&&(hit1.getScintillator() != hit2.getScintillator())
			) {
				for(int thr=1;thr<=4;thr++){
					double tof = fabs( JPetHitUtils::getTimeAtThr(hit1, thr) - JPetHitUtils::getTimeAtThr(hit2, thr));
					tof /= 1000.; // [ns]
					if( tof < 100.0 ){
						int delta_ID = fBarrelMap.calcDeltaID(hit1, hit2);
						fillDeltaIDhisto(delta_ID, thr, hit1.getBarrelSlot().getLayer());
					}
				}
			}
		}
	}
}
void TaskSyncStrips::terminate(){}
void TaskSyncStrips::fillDeltaIDhisto(int delta_ID, int threshold, const JPetLayer & layer){
	int layer_number = fBarrelMap.getLayerNumber(layer);
	string histo_name = "Delta_ID_for_coincidences_"+LayerThr(layer_number,threshold);
	getStatistics().getHisto1D(histo_name.c_str()).Fill(delta_ID);
}
void TaskSyncStrips::setWriter(JPetWriter* writer){fWriter =writer;}
