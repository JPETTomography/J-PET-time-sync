// this file is distributed under 
// MIT license
#include <JPetManager/JPetManager.h>
#include <JPetTaskLoader/JPetTaskLoader.h>
#include "PreselectionMain.h"
using namespace std;
int main(int argc, char* argv[]) {
	if(argc==1){
		char * frameworkargs[2];
		frameworkargs[0]=argv[0];
		frameworkargs[1]="--help";
		cout<<"AUTOMATICALLY GIVEN OPTIONS -l and -i, don't give them once more"<<endl;
		JPetManager::getManager().parseCmdLine(2, frameworkargs);
	}else{
		char * frameworkargs[argc+4];
		frameworkargs[0]=argv[0];
		frameworkargs[1]="-l";
		frameworkargs[2]="j-pet-setup.json";
		frameworkargs[3]="-i";
		frameworkargs[4]="43";
		for(size_t i=1;i<argc;i++)frameworkargs[i+4]=argv[i];
		JPetManager::getManager().parseCmdLine(argc+4, frameworkargs);
		AddModules();
		JPetManager::getManager().run();
	}
}
