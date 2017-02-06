// this file is distributed under 
// MIT license
#include <iostream>
#include <string>
#include <JPetLargeBarrelExtensions/PetDict.h>
#include <JPetLargeBarrelExtensions/TimeSyncDeltas.h>
#include <Calc/convention.h>
using namespace std;
void errmsg(const string&myname){
	cerr<<"Usage for generating report:"<<endl
	<<"(cat DeltaA.txt;cat DeltaB.txt;cat DeltaC.txt;cat DeltaD.txt)|"
	<<myname<<"out >> report.txt"<<endl
	<<"For parsing report:"<<endl
	<<"(cat report.txt)|"<<myname<<" in 1 >> DeltaA.txt"<<endl
	<<"(cat report.txt)|"<<myname<<" in 2 >> DeltaB.txt"<<endl
	<<"(cat report.txt)|"<<myname<<" in 3 >> DeltaC.txt"<<endl
	<<"(cat report.txt)|"<<myname<<" in 4 >> DeltaD.txt"<<endl;
}
int main(int argc, char **argv) {
    if((argc<2)&&(argc>3)){
	errmsg(argv[0]);
	return -1;
    }
    const string opt=argv[1];
    if(opt=="out"){
	if((argc==3)){
	    errmsg(argv[0]);
	    return -1;
	}
	const vector<string> Thr{"1","2","3","4"};
	string tail="\t0\t0\t0";
	for(const auto&thr:Thr){
	    const auto D=make_JPetMap<SynchroStrip>();
	    cin>>(*D);
	    for(size_t layer=1;layer<=D->layersCount();layer++){
		for(size_t slot=1;slot<=D->layerSize(layer);slot++){
		    const auto&item=D->item({.layer=layer,.slot=slot});
		    cout<<layer<<"\t"<<slot<<"\t"
			<<"A\t"<<thr<<"\t"
			<<item.A<<"\t"<<item.dA
			<<tail<<endl;
		    cout<<layer<<"\t"<<slot<<"\t"
			<<"B\t"<<thr<<"\t"
			<<item.B<<"\t"<<item.dB
			<<tail<<endl;
		}
	    }
	}
	return 0;
    }
    if(opt=="in"){
	if((argc==2)){
	    errmsg(argv[0]);
	    return -1;
	}
	const string THR=argv[2];
	const auto D=make_JPetMap<SynchroStrip>();
	size_t counter=0;
	while(counter<(D->slotsCount()*2)){
	    size_t layer,slot;
	    string side,thr,tail1,tail2,tail3;
	    double offs,doffs;
	    cin>>layer>>slot>>side>>thr>>offs>>doffs>>tail1>>tail2>>tail3;
	    if(THR==thr){
		auto&item=D->item({.layer=layer,.slot=slot});
		if(side=="A"){
		    counter++;
		    item.A=offs;
		    item.dA=doffs;
		}
		if(side=="B"){
		    counter++;
		    item.B=offs;
		    item.dB=doffs;
		}
	    }
	}
	cout<<(*D);
	return 0;
    }
    cerr<<"unrecognized option "<<opt<<endl;
    errmsg(argv[0]);
    return 1;
}
