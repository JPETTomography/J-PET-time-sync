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
    stream<<"layer_"<<layer<<"_thr_"<<thr;
    return stream.str();
}
const string LayerSlotThr(const size_t layer, const size_t slot, const size_t thr){
    stringstream stream;
    stream<<"layer_"<<layer<<"_slot_"<<slot<<"_thr_"<<thr;
    return stream.str();
}


const hist<double> ReadHist(const string&filename,const string&histname){
    //ToDo: find and fix memory leak :(
    hist<double> points;
    const TFile* file=TFile::Open(filename.c_str());
    if(file){
	const THashTable* list=dynamic_cast<THashTable*>(const_cast<TFile*>(file)->Get("Stats"));
	if(list){
	    const TH1F* histogram=dynamic_cast<TH1F*>(list->FindObject(histname.c_str()));
	    if(histogram){
		for(int i=1,N=histogram->GetNbinsX();i<=N;i++){
		    const double x=histogram->GetBinCenter(i),
		    dx=histogram->GetBinWidth(i)/2.0,
		    y=histogram->GetBinContent(i);
		    points << point<value<double>>({x,dx},value<double>::std_error(y));
		}
		delete histogram;
	    }else throw Exception<TH1F>("No histogram "+histname);
	    delete list;
	}else throw Exception<TDirectoryFile>("No hash table Stats");
	const_cast<TFile*>(file)->Close();
	delete file;
    }else throw Exception<TFile>("No file "+filename);
    return points;
}
const hist<double> ReadHist(const vector< string >& filenames,const string& histname){
    hist<double> result;
    for(const string&filename:filenames){
	const auto H=ReadHist(filename,histname);
	if(result.size()>0)result.imbibe(H);
	else result=H;
    }
    return result;
}
