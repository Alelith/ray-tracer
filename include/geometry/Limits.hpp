#ifndef LIMITS_HPP
# define LIMITS_HPP

#include <cmath>
#include <algorithm>

class Limits
{
	private:
		double	min;
		double	max;
	public:
		Limits();
		Limits(double min, double max);
		~Limits();

		double	getMin() const;
		double	getMax() const;

		bool	isBetween(double t) const;
		Limits	expand(double delta) const;
};

Limits	getInterval(Limits a, Limits b);

#endif /* LIMITS_HPP */