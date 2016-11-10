// this file is distributed under 
// MIT license
#include <iostream>
#include <math_h/error.h>
#include "SyncProcedures.h"
using namespace std;
using namespace MathTemplates;
namespace Graph{
    ConnectionChecker::ConnectionChecker(const size_t count){
	for(size_t i=0;i<count;i++)
	    f_data.push_back(list<size_t>());
    }
    Graph::ConnectionChecker::~ConnectionChecker(){}
    void ConnectionChecker::Connect(const size_t a, const size_t b){
	if(!connected(a,b)){
	    f_data[a].push_back(b);
	    f_data[b].push_back(a);
	}
    }
    const bool Graph::ConnectionChecker::connected(const size_t a, const size_t b) const{
	if((a>=f_data.size())||(b>=f_data.size()))
	    throw Exception<ConnectionChecker>("index range check error");
	for(const size_t i:f_data[a])
	    if(i==b)return true;
	return false;
    }
    const std::list<size_t> & ConnectionChecker::directly_connected_to(const size_t number) const{
	if(number>=f_data.size())
	    throw Exception<ConnectionChecker>("index range check error");
	return f_data[number];
    }
    const std::list<size_t> ConnectionChecker::connected_to(const size_t number) const{
	list<size_t> to_check{number},result{};
	while(to_check.size()>0){
	    size_t index=to_check.front();
	    to_check.pop_front();
	    result.push_back(index);
	    for(const size_t i:directly_connected_to(index)){
		bool exists=false;
		for(const size_t j:result)if(j==i)exists=true;
		if(!exists)for(const size_t j:to_check)if(j==i)exists=true;
		if(!exists)to_check.push_back(i);
	    }
	}
	return result;
    }


};
