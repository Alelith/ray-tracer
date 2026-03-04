#include "Limits.hpp"

Limits::Limits() : min(0.0), max(0.0) {}

Limits::Limits(double min, double max)
{
	this->min = min;
	this->max = max;
}


double	Limits::getMin() const { return min; }

double	Limits::getMax() const { return max; }

bool	Limits::isBetween(double t) const
{ return t >= min && t <= max; }

Limits	Limits::expand(double delta) const
{ return Limits(min - delta, max + delta); }

Limits	getInterval(const Limits &a, const Limits &b)
{
	double newMin = std::min(a.getMin(), b.getMin());
	double newMax = std::max(a.getMax(), b.getMax());
	return Limits(newMin, newMax);
}
