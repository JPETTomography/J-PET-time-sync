// this file is distributed under 
// MIT license
#include <iostream>
#include "SyncProcedures.h"
using namespace std;
namespace Graph{
    ConnectionChecker::ConnectionChecker(const size_t count){
	for(size_t i=0;i<count;i++)
	    f_data.push_back(vector<size_t>());
    }
    ConnectionChecker::~ConnectionChecker(){}
    void ConnectionChecker::Connect(const size_t a, const size_t b){
	if(!connected(a,b)){
	    f_data[a].push_back(b);
	    f_data[b].push_back(a);
	}
    }
    const bool ConnectionChecker::connected(const size_t a, const size_t b)const{
	if((a>=f_data.size())||(b>=f_data.size()))
	    throw Exception<ConnectionChecker>("index range check error");
	for(const size_t i:f_data[a])
	    if(i==b)return true;
	return false;
    }
    const std::vector<size_t>&ConnectionChecker::directly_connected_to(const size_t number)const{
	if(number>=f_data.size())
	    throw Exception<ConnectionChecker>("index range check error");
	return f_data[number];
    }
    const std::vector<size_t> ConnectionChecker::connected_to(const size_t number)const{
	vector<size_t> result{number};
	size_t current_index=0;
	while(current_index<result.size()){
	    for(const size_t&item:directly_connected_to(result[current_index])){
		bool exists=false;
		for(size_t i=0,n=result.size();(!exists)&&(i<n);i++)
		    if(result[i]==item)exists=true;
		if(!exists)
		    result.push_back(item);
	    }
	    current_index++;
	}
	return result;
    }
};
