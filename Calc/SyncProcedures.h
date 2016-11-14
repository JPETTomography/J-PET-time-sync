// this file is distributed under 
// MIT license
#ifndef ___________SYNC_AB___________________
#       define ___________SYNC_AB___________________
#include <utility>
#include <vector>
#include <list>
#include <math_h/hists.h>
#include "convention.h"
namespace Sync{
    const SyncAB_results Fit4SyncAB(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads);
    const SyncOposite_results Fit4SyncOposite(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads);
    const SyncNeighbour_results Fit4SyncNeighbour(const MathTemplates::hist<double>&hist, const std::string&displayname,const size_t threads);
};
namespace Graph{
    class ConnectionChecker{
    public:
	ConnectionChecker(const size_t count);
	virtual ~ConnectionChecker();
	void Connect(const size_t a,const size_t b);
	const bool connected(const size_t a,const size_t b)const;
	const std::vector<size_t>&directly_connected_to(const size_t number)const;
	const std::vector<size_t> connected_to(const size_t number)const;
    private:
	std::vector<std::vector<size_t>> f_data;
    };
};
#endif
