#include "cycloid.h"
#include <math.h>

using namespace geometric;

double point::length()
{
	return sqrt(X*X+Y*Y);
}

cycloid::cycloid(double radius)
{
	if(radius < 0) throw IncorrectRadiusException("Radius must be positive or zero");
	_radius = radius;
}

point cycloid::coords(double fi)
{
	return {_radius*(fi-sin(fi)),_radius*(1-cos(fi))};
	}

double cycloid::curvature(double fi)
{
	return -4*abs(sin((fi)/2));
}

double cycloid::length(double fi)
{
	return abs(-4*_radius*(cos(fi/2)-1));	
}
double cycloid::area()
{
	return PI*3*_radius*_radius;	
}
double cycloid::surface_area()
{
	return 64.0*PI*_radius*_radius/3;
}
double cycloid::volume()
{
	return 5.0*PI*PI*_radius*_radius*_radius;
}