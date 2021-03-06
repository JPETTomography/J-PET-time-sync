// this file is distributed under 
// MIT license
#ifndef ________GETHIST_H___________
#	define ________GETHIST_H___________
#include <string>
#include <math_h/hists.h>
const MathTemplates::hist<double> ReadHist(const std::string&filename,const std::string&histname);
const MathTemplates::hist<double> ReadHist(const std::vector<std::string>&filenames,const std::string&histname);
#endif
