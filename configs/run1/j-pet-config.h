// this file is distributed under 
// MIT license
#ifndef ________J_PET_CONFIG_H_________
#	define ________J_PET_CONFIG_H_________
//run number that must be in agreement with json file
#	define JSON_RUN_NUMBER "43"
//time unit constant used to obtain time from DAQ values
#	define DAQ_2_TIME_UNIT 1.0
//time unit constant. 1.0 means ns; 1000.0 means ps...
#	define TIME_UNIT_CONST 1.0
//time diff histogram range: bins_count,from,to
#	define TIME_HISTO_RANGE 120,-30.,+30.
//time spectra plots options
#	define TIME_PLOT_OPTS "set xrange [-30:30]"
//solving equations initial parameters dispersion around zero
#	define SOLVING_EQ_PARAM_SIGMA 10.
//solving equations mutations constant (view formula in Solve-equations.cpp)
#	define SOLVING_EQ_MUTATIONS 0.000001
//solving equation mutations time decay (view formula in Solve-equations.cpp)
#	define SOLVING_EQ_MUTATIONS_TAU 3000.
//output plots options for Solve-equations utility
#	define OUT_PLOT_OPTS "set yrange [-30:30]"
//size of J-PET detector for structures
//that store sets of strips parameters
//this must be in agreement with configuration in json file
#	define Full_J_PET_SIZE {48,48,96}
//half-size of J-PET detector for storing opposite
//coincidences hit time difference spectra
#	define Half_J_PET_SIZE {24,24,48}
//size of structure used for inter-layer synchronization
// there must be all layers except the last one
#	define InterLayer_J_PET_SIZE {48,48}
//Indices for inter-layer synchronization
//the format is the following:
// {<for layer 1>},{<for layer 2>},...
//layers count must fit previously defined structure size
// each layer item inside brackets has the following format
// {item1},{item2},...
// where items count can be arbitrary
// item format is the following:
//  .coef=C,.offs=O
// where C and O are some numbers
// the item corresponds to the coincidences between strip
// number i in current layer and strip number i*C+O in the next layer
#	define InterLayer_Indices {{.coef=1,.offs=1},{.coef=1,.offs=48-2}},{{.coef=2,.offs=4},{.coef=2,.offs=96-4}}
#endif
