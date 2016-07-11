#ifndef ___________CONV_JPET_TIME_SYNC________
#	define ___________CONV_JPET_TIME_SYNC________
#include <math_h/sigma.h>
struct PositionFromFit{
	MathTemplates::value<double> pos,sigma;
	double chisq;
};
#endif