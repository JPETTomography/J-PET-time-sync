#include <algorithm>
#include <sstream>
#include <math_h/error.h>
#include "BarrelExtensions.h"
using namespace std;
using namespace MathTemplates;
const string Layer(const size_t layer){
    stringstream stream;
    stream<<"layer"<<layer;
    return stream.str();
}
const string LayerSlot(const size_t layer, const size_t slot){
    stringstream stream;
    stream<<"layer"<<layer<<"-slot"<<slot;
    return stream.str();
}
const string LayerSlotThr(const size_t layer, const size_t slot, const size_t thr){
    stringstream stream;
    stream<<"layer"<<layer<<"-slot"<<slot<<"-thr"<<thr;
    return stream.str();
}

const StripPos AbstractBarrelMapping::getStripPos(const JPetBarrelSlot & slot) const {
    return {.layer=getLayerNumber(slot.getLayer()),.slot=getSlotNumber(slot)};
}
const vector< size_t > AbstractBarrelMapping::getLayersSizes() const{
    vector<size_t> res;
    for(size_t i=1,n=getLayersCount();i<=n;i++)
	res.push_back(getSlotsCount(i));
    return res;
}

const double degFullCircle = 360.;
LargeBarrelMapping::LargeBarrelMapping(const JPetParamBank& paramBank){
    for(auto & layer : paramBank.getLayers() ){
	double radius = layer.second->getRadius();
	fRadii.push_back(radius);
	fTheta.push_back(vector<double>());
    }
    sort( fRadii.begin(), fRadii.end(),less<double>() );
    for(const auto & slot : paramBank.getBarrelSlots()){
	const int layer_number = getLayerNumber( slot.second->getLayer() );
	fTheta[layer_number-1].push_back(slot.second->getTheta());
    }
    for(auto & thetas : fTheta)
	sort( thetas.begin(), thetas.end(), less<double>() );
}
LargeBarrelMapping::~LargeBarrelMapping(){}
const size_t LargeBarrelMapping::getLayersCount() const{
    return fRadii.size();
}
const size_t LargeBarrelMapping::getLayerNumber(const JPetLayer& layer) const{
    size_t res=0;
    while(layer.getRadius()!=fRadii[res])
	res++;
    return res+1;
}
const size_t LargeBarrelMapping::getSlotsCount(const size_t layer) const{
    return fTheta[layer-1].size();
}
const size_t LargeBarrelMapping::getSlotsCount(const JPetLayer& layer) const{
    return fTheta[getLayerNumber(layer)-1].size();
}
const size_t LargeBarrelMapping::getSlotNumber(const JPetBarrelSlot& slot) const{
    const auto&theta=fTheta[getLayerNumber(slot.getLayer())-1];
    size_t res=0;
    while(slot.getTheta()!=theta[res])
	res++;
    return res+1;
}
const size_t LargeBarrelMapping::calcDeltaID(const JPetBarrelSlot& slot1,const JPetBarrelSlot& slot2) const{
    if(slot1.getLayer().getId()==slot2.getLayer().getId()){
	auto delta_ID = size_t(abs(int(getSlotNumber(slot1))-int(getSlotNumber(slot2))));
	auto layer_size = getSlotsCount(slot1.getLayer());
	auto half_layer_size = layer_size/2;
	if( delta_ID > half_layer_size ) return layer_size-delta_ID;
	return delta_ID;
    }
    throw Exception<LargeBarrelMapping>("attempt to calc deltaID for strips from different layers");
}
const size_t LargeBarrelMapping::calcGlobalPMTNumber(const JPetPM & pmt) const {
    const size_t number_of_sides = 2;
    const auto layer_number = getLayerNumber(pmt.getBarrelSlot().getLayer());
    size_t pmt_no = 0;
    for(size_t l=1;l<layer_number;l++)
	pmt_no += number_of_sides * fTheta[l-1].size();
    const auto slot_number = getSlotNumber(pmt.getBarrelSlot());
    if( pmt.getSide() == JPetPM::SideB )
	pmt_no += fTheta[layer_number-1].size();
    pmt_no += slot_number - 1;
    return pmt_no;
}
LargeBarrelTask::LargeBarrelTask(const char * name, const char * description)
:JPetTask(name, description){}
LargeBarrelTask::~LargeBarrelTask(){}
void LargeBarrelTask::init(const JPetTaskInterface::Options&){
    fBarrelMap=make_shared<LargeBarrelMapping>(getParamBank());
}
void LargeBarrelTask::setWriter(JPetWriter* writer){fWriter=writer;}
JPetWriter&LargeBarrelTask::writter() const{
    //ToDo: provide control
    return *fWriter;
}
const std::shared_ptr<LargeBarrelMapping>LargeBarrelTask::map() const{
    return fBarrelMap;
}
void TOT_Hists::init(const JPetTaskInterface::Options& opts){
    LargeBarrelTask::init(opts);
}
TOT_Hists::TOT_Hists(const char* name, const char* description)
:LargeBarrelTask(name,description){}
TOT_Hists::~TOT_Hists(){}
void TOT_Hists::createTOTHistos(const std::string& suffix){
    for(auto & layer : getParamBank().getLayers()){
	const auto ln=map()->getLayerNumber(*layer.second);
	for(size_t sl=1,n=map()->getSlotsCount(ln);sl<=n;sl++)
	    for(size_t thr=1;thr<=4;thr++){
		getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(ln,sl,thr)+"-A-"+suffix).c_str(), "",500, 0.,100.));
		getStatistics().createHistogram( new TH1F(("TOT-"+LayerSlotThr(ln,sl,thr)+"-B-"+suffix).c_str(), "",500, 0.,100.));
	    }
    }
}
void TOT_Hists::fillTOTHistos(const JPetHit& hit, const std::string& suffix){
    const auto strip1=map()->getStripPos(hit.getBarrelSlot());
    auto TOTA=hit.getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber(),
	TOTB=hit.getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber();
    for(size_t thr=1;thr<=4;thr++){
	getStatistics().getHisto1D(("TOT-"+LayerSlotThr(strip1.layer,strip1.slot,thr)+"-A-"+suffix).c_str()).Fill(TOTA[thr]);
	getStatistics().getHisto1D(("TOT-"+LayerSlotThr(strip1.layer,strip1.slot,thr)+"-B-"+suffix).c_str()).Fill(TOTB[thr]);
    }
}

const TOTs TOT_Hists::getTOTs(const JPetHit& hit) const{
    auto TOTA=hit.getSignalA().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber(),
    	TOTB=hit.getSignalB().getRecoSignal().getRawSignal().getTOTsVsThresholdNumber();
    TOTs result;
    for(size_t thr=1;thr<=4;thr++){
	result.A[thr-1]=TOTA[thr];
	result.B[thr-1]=TOTB[thr];
    }
    return result;
}

