#include "Limits.cuh"

RT_CALLABLE Limits::Limits() : min(0.0001), max(INFINITY) {}

RT_CALLABLE Limits::Limits(float min, float max)
{
	this->min = min;
	this->max = max;
}
