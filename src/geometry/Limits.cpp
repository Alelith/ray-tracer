#include "Limits.hpp"

Limits::Limits() : min(0.0001), max(INFINITY) {}

Limits::Limits(float min, float max)
{
	this->min = min;
	this->max = max;
}

float	Limits::getMin() const { return min; }

float	Limits::getMax() const { return max; }

float	Limits::getCenter() const { return (min + max) / 2.0f; }

float	Limits::getLength() const { return max - min; }

float	Limits::clamp(float t) const
{
	return std::max(min, std::min(max, t));
}

bool	Limits::isBetween(float t) const
{ return t >= min && t <= max; }

Limits	Limits::expand(float delta) const
{ return Limits(min - delta, max + delta); }

Limits	getInterval(const Limits &a, const Limits &b)
{
	float newMin = std::min(a.getMin(), b.getMin());
	float newMax = std::max(a.getMax(), b.getMax());
	return Limits(newMin, newMax);
}
