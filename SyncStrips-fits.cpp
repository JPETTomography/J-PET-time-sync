#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
#include <IO/gethist.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
    if(argc<3){
	cerr<<"Usage: "<<argv[0]<<" <thread_count> <filename> <filename> ..."<<endl;
	return -1;
    }
    string parameters_file(argv[1]);
    vector<string> root_filenames;
    for(int i=2;i<argc;i++)
	root_filenames.push_back(string(argv[i]));
    Plotter::Instance().SetOutput(".","strips-synchro");
    return 0;
}
