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
const hist<double> ReadHist(const string&filename,const string&histname){
    //ToDo: find and fix memory leak :(
    hist<double> points;
    TFile*file=TFile::Open(filename.c_str());
    if(file){
	THashTable* list=dynamic_cast<THashTable*>(file->Get("Stats"));
	if(list){
	    list->SetOwner(kTRUE);
	    TH1F* histogram=dynamic_cast<TH1F*>(list->FindObject(histname.c_str()));
	    if(histogram){
		for(int i=1,N=histogram->GetNbinsX();i<=N;i++){
		    const double x=histogram->GetBinCenter(i),
		    dx=histogram->GetBinWidth(i)/2.0,
		    y=histogram->GetBinContent(i);
		    points << point<value<double>>({x,dx},value<double>::std_error(y));
		}
	    }else throw Exception<TH1F>("No histogram "+histname);
	    delete list;    
	}else throw Exception<THashTable>("No hash table Stats");
	file->Close();
    }else throw Exception<TFile>("File "+filename+" not found");
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
