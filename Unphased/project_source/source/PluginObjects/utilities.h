#pragma once
#include <cfloat>
#include <cmath>

inline bool isFloatEqual(const double a, const double b)
{
	return fabs(a - b) < FLT_EPSILON;
}
