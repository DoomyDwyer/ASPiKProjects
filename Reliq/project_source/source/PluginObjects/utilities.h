#pragma once
#include <cfloat>
#include <cmath>

inline bool isEqual(const float a, const float b)
{
	return fabs(a - b) < FLT_EPSILON;
}

inline bool isEqual(const double a, const double b)
{
	return fabs(a - b) < FLT_EPSILON;
}
