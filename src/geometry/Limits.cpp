#include "Limits.hpp"

Limits::Limits() : min(0.0001), max(INFINITY) {}

Limits::Limits(float min, float max)
{
	this->min = min;
	this->max = max;
}
