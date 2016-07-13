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
#include "TaskE.h"

TaskE::TaskE(const char * name, const char * description)
:JPetTask(name, description){}

void TaskE::init(const JPetTaskInterface::Options& opts){
	fBarrelMap.buildMappings(getParamBank());
	for(auto & layer : getParamBank().getLayers()){
		for (int thr=1;thr<=4;thr++){
			char * histo_name = Form("Delta_ID_for_coincidences_layer_%d_thr_%d", fBarrelMap.getLayerNumber(*layer.second), thr);
			char * histo_title = Form("%s;#Delta ID", histo_name); 
			int n_slots_in_half_layer = fBarrelMap.getNumberOfSlots(*layer.second) / 2;
			getStatistics().createHistogram( new TH1F(histo_name, histo_title,n_slots_in_half_layer, 0.5, n_slots_in_half_layer+0.5));
			histo_name = Form("TOF_vs_Delta_ID_layer_%d_thr_%d", fBarrelMap.getLayerNumber(*layer.second), thr);
			histo_title = Form("%s;#Delta ID;TOF [ns]", histo_name); 
			getStatistics().createHistogram( new TH2F(histo_name, histo_title,n_slots_in_half_layer, 0.5, n_slots_in_half_layer+0.5,100, 0., 15.));
			for(char side : {'A', 'B'} ){
				histo_name = Form("TOT_vs_TOT_layer_%d_thr_%d_side_%c", fBarrelMap.getLayerNumber(*layer.second), thr, side);
				histo_title = Form("%s;TOT [ns];TOT [ns]", histo_name); 
				getStatistics().createHistogram( new TH2F(histo_name, histo_title, 120, 0., 120., 120, 0., 120.));
			}
			
		}
	}
}
void TaskE::exec(){
	JPetHit currHit = (JPetHit&) (*getEvent());
	if (fHits.empty()) {
		fHits.push_back(currHit);
	} else {
		if (fHits[0].getTimeWindowIndex() == currHit.getSignalB().getTimeWindowIndex()) {
			fHits.push_back(currHit);
		} else {
			fillCoincidenceHistos(fHits);
			fHits.clear();
			fHits.push_back(currHit);
		}
	}
}
void TaskE::fillCoincidenceHistos(std::vector<JPetHit>& hits){
	for (auto i = hits.begin(); i != hits.end(); ++i) {
		for (auto j = i; ++j != hits.end(); ) {
			JPetHit & hit1 = *i;
			JPetHit & hit2 = *j;
			if (
				(hit1.getBarrelSlot().getLayer() == hit2.getBarrelSlot().getLayer())
				&&(hit1.getScintillator() != hit2.getScintillator())
			) {
				for(int thr=1;thr<=4;thr++){
					if( isGoodTimeDiff(hit1, thr) && isGoodTimeDiff(hit2, thr) ){
						double tof = fabs( JPetHitUtils::getTimeAtThr(hit1, thr) -
						JPetHitUtils::getTimeAtThr(hit2, thr)
						);
						tof /= 1000.; // [ns]
						if( tof < 50.0 ){
							int delta_ID = fBarrelMap.calcDeltaID(hit1, hit2);
							fillDeltaIDhisto(delta_ID, thr, hit1.getBarrelSlot().getLayer());
							fillTOFvsDeltaIDhisto(delta_ID, thr, hit1, hit2);
							fillTOTvsTOThisto(delta_ID, thr, hit1, hit2);
						}
					}
				}
			}
		}
	}
}
void TaskE::terminate(){}
const char * TaskE::formatUniqueSlotDescription(const JPetBarrelSlot & slot, int threshold, const char * prefix = ""){
	int slot_number = fBarrelMap.getSlotNumber(slot);
	int layer_number = fBarrelMap.getLayerNumber(slot.getLayer()); 
	return Form("%slayer_%d_slot_%d_thr_%d",prefix,layer_number,slot_number,threshold);
}
void TaskE::fillDeltaIDhisto(int delta_ID, int threshold, const JPetLayer & layer){
	int layer_number = fBarrelMap.getLayerNumber(layer);
	const char * histo_name = Form("Delta_ID_for_coincidences_layer_%d_thr_%d", layer_number, threshold);
	getStatistics().getHisto1D(histo_name).Fill(delta_ID);
}
void TaskE::fillTOFvsDeltaIDhisto(int delta_ID, int thr, const JPetHit & hit1, const JPetHit & hit2){
	int layer_number = fBarrelMap.getLayerNumber(hit1.getBarrelSlot().getLayer());
	const char * histo_name = Form("TOF_vs_Delta_ID_layer_%d_thr_%d",fBarrelMap.getLayerNumber(hit1.getBarrelSlot().getLayer()),thr);
	double tof = fabs( JPetHitUtils::getTimeAtThr(hit1, thr) - JPetHitUtils::getTimeAtThr(hit2, thr));
	tof /= 1000.; // to have the TOF in ns instead of ps
	getStatistics().getHisto2D(histo_name).Fill(delta_ID, tof);
}
bool TaskE::isGoodTimeDiff(const JPetHit & hit, int thr){
	double mean_timediff = getAuxilliaryData().getValue("timeDiffAB mean values",formatUniqueSlotDescription(hit.getBarrelSlot(),thr, "timeDiffAB_"));
	double this_hit_timediff = JPetHitUtils::getTimeDiffAtThr(hit, thr) / 1000.; // [ns]
	if( fabs( this_hit_timediff - mean_timediff ) < 1.0 )return true;
	else return false;
}

void TaskE::fillTOTvsTOThisto(int delta_ID, int thr, const JPetHit & hit1, const JPetHit & hit2){
	int n_slots_in_half_layer = fBarrelMap.getNumberOfSlots(hit1.getBarrelSlot().getLayer()) / 2;
	if( delta_ID != n_slots_in_half_layer )return;
	double totA1 = hit1.getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber().at(thr);
	double totB1 = hit1.getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber().at(thr);
	double totA2 = hit2.getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber().at(thr);
	double totB2 = hit2.getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber().at(thr);
	char * histo_name;
	histo_name = Form("TOT_vs_TOT_layer_%d_thr_%d_side_A",fBarrelMap.getLayerNumber(hit1.getBarrelSlot().getLayer()), thr);  
	getStatistics().getHisto2D(histo_name).Fill(totA1/1000., totA2/1000.);
	histo_name = Form("TOT_vs_TOT_layer_%d_thr_%d_side_B",fBarrelMap.getLayerNumber(hit1.getBarrelSlot().getLayer()), thr);  
	getStatistics().getHisto2D(histo_name).Fill(totB1/1000., totB2/1000.);
}
void TaskE::setWriter(JPetWriter* writer){fWriter =writer;}
