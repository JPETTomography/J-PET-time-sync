// this file is distributed under 
// MIT license
#ifndef ___________SYNC_AB___________________
#       define ___________SYNC_AB___________________
#include <utility>
#include <math_h/hists.h>
#include "convention.h"
namespace Sync{
  const SyncAB_results Fit4SyncAB(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads);
  const SyncOposite_results Fit4SyncOposite(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads);
  const SyncNeighbour_results Fit4SyncNeighbour(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads);
};
#endif