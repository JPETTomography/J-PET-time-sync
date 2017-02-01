// this file is distributed under 
// MIT license
#ifndef ________J_PET_CONFIG_H_________
#	define ________J_PET_CONFIG_H_________

//ATTENTION: please note that all sizes defined in 
//this file must be in agreement with other parts
// of configuration

//run number that must be in agreement with json file
#	define JSON_RUN_NUMBER "43"
//size of J-PET detector for structures
//that store sets of strips parameters
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
