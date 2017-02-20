// this file is distributed under 
// MIT license
#ifndef ________J_PET_CONFIG_H_________
#	define ________J_PET_CONFIG_H_________
#	define JSON_RUN_NUMBER "2"
#	define DAQ_2_TIME_UNIT 1.0
#	define TIME_UNIT_CONST 1.0
#	define TIME_HISTO_RANGE 120,-30.,+30.
#	define TIME_PLOT_OPTS "set xrange [-30:30]"
#	define SOLVING_EQ_PARAM_SIGMA 20.
#	define SOLVING_EQ_MUTATIONS 0.000001
#       define SOLVING_EQ_MUTATIONS_TAU 5000.
#	define OUT_PLOT_OPTS "set yrange [-30:30]"
#	define Full_J_PET_SIZE {48,48,96}
#	define Half_J_PET_SIZE {24,24,48}
#	define InterLayer_J_PET_SIZE {48,48}
#	define InterLayer_Indices {{.coef=1,.offs=1},{.coef=1,.offs=48-2}},{{.coef=2,.offs=4},{.coef=2,.offs=96-4}}
#endif
