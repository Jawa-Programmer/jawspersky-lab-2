#include "cycloid.h"
#include <math.h>

using namespace geometric;

double point::length()
{
	return sqrt(X*X+Y*Y);
}

cycloid::cycloid(double radius, double phase)
{
	if(radius < 0) throw IncorrectRadiusException("Radius must be positive or zero");
	_radius = radius;
	_phase = phase;
}

point cycloid::coords(double fi)
{
	return {_radius*(fi-_phase-sin(fi-_phase)),_radius*(1-cos(fi-_phase))};
	}

double cycloid::curvature(double fi)
{
	return -4*abs(sin((fi - _phase)/2));
}

double cycloid::length(double fi)
{
	return abs(-4*_radius*(cos(fi/2)-cos(_phase/2)));	
}
double cycloid::area()
{
	return PI*3*_radius;	
}