// this file is distributed under 
// MIT license
#ifndef ___________SYNC_AB___________________
#	define ___________SYNC_AB___________________
#include <utility>
#include <math_h/hists.h>
#include "convention.h"
namespace SyncAB{
	const PositionFromFit Fit4SyncAB(const MathTemplates::hist<double>&hist, const std::string&&displayname);
};
#endif