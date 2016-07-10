// this file is distributed under 
// MIT license
#ifndef ________GETHIST_H___________
#	define ________GETHIST_H___________
#include <string>
#include <math_h/hists.h>
MathTemplates::hist<double> ReadHist(const std::string&filename,const std::vector<std::string>&path,const std::string&histname);
#endif