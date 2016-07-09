#include <iostream>
#include <sstream>
#include <map>
#include <gnuplot_wrap.h>
using namespace std;
using namespace GnuplotWrap;
using namespace MathTemplates;
int main(int argc, char **argv) {
	Plotter::Instance().SetOutput(".","test");

	return 1;
}
