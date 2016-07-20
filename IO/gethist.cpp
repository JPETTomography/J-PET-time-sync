// this file is distributed under 
// MIT license
#include <math.h>
#include <sstream>
#include <TObject.h>
#include <TH1F.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <THashTable.h>
#include <TTree.h>
#include <TDirectoryFile.h>
#include <math_h/error.h>
#include "gethist.h"
using namespace std;
using namespace MathTemplates;
const string LayerThr(const size_t layer, const size_t thr){
	stringstream stream;
	stream<<"layer_"<<layer;
	stream<<"_thr_"<<thr;
	return stream.str();
}
const string LayerSlotThr(const size_t layer, const size_t slot, const size_t thr){
	stringstream stream;
	stream<<"layer_"<<layer;
	stream<<"_slot_"<<slot;
	stream<<"_thr_"<<thr;
	return stream.str();
}


hist<double> ReadHist(const string&filename,const string&histname){
	hist<double> points;
	TFile* file=TFile::Open(filename.c_str());
	if(file){
		THashTable* list=dynamic_cast<THashTable*>(dynamic_cast<TDirectoryFile*>(file)->Get("Stats"));
		if(list){
			TH1F* histogram=dynamic_cast<TH1F*>(list->FindObject(histname.c_str()));
			if(histogram){
				for(int i=1,N=histogram->GetNbinsX();i<=N;i++){
					double y=histogram->GetBinContent(i);
					double dy=sqrt(y);
					if(dy<1.0)
						dy=1.0;
					double x=histogram->GetBinCenter(i);
					double dx=histogram->GetBinWidth(i)/2.0;
					points<<point<value<double>>({x,dx},{y,dy});
				}
			}else throw Exception<TH1F>("No histogram "+histname);
		}else throw Exception<TDirectoryFile>("No hash table Stats");
		file->Close();
		delete file;
	}else throw Exception<TFile>("No file "+filename);
	return points;
}
hist< double > ReadHist(const vector< string >& filenames,const string& histname){
	hist<double> result;
	for(const string&filename:filenames){
		auto H=ReadHist(filename,histname);
		if(result.size()>0)result.imbibe(H);
		else result=H;
	}
	return result;
}

