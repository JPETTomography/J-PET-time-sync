// this file is distributed under 
// MIT license
#include <math.h>
#include <TObject.h>
#include <TH1F.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TDirectoryFile.h>
#include "gethist.h"
using namespace std;
using namespace MathTemplates;
hist<double> ReadHist(const string&filename,const vector<string>&path,const string&histname){
	vector<hist<double>::Point> points;
	TFile* file=TFile::Open(filename.c_str());
	if(file){
		TDirectoryFile* dir1=file;
		for(string name:path){
			TDirectoryFile* dir2=dynamic_cast<TDirectoryFile*>(dir1->Get(name.c_str()));
			if(dir2)
				dir1=dir2;
		}
		TH1F* histogram=dynamic_cast<TH1F*>(dir1->Get(histname.c_str()));
		if(histogram){
			for(int i=1,N=histogram->GetNbinsX();i<=N;i++){
				double y=histogram->GetBinContent(i);
				double dy=sqrt(y);
				if(dy<1.0)
					dy=1.0;
				double x=histogram->GetBinCenter(i);
				double dx=histogram->GetBinWidth(i)/2.0;
				points.push_back(point<value<double>>({x,dx},{y,dy}));
			}
		}
		file->Close();
		delete file;
	}
	return hist<double>(points);
}
