#ifndef LIMITS_HPP
# define LIMITS_HPP

#include <cmath>
#include <algorithm>

class Limits
{
	private:
		float	min;
		float	max;
	public:
		Limits();
		Limits(float min, float max);
		~Limits() = default;

		float	getMin() const;
		float	getMax() const;

		float	getCenter() const;
		float	getLength() const;

		bool	isBetween(float t) const;
		float	clamp(float t) const;
		Limits	expand(float delta) const;
};

Limits	getInterval(const Limits &a, const Limits &b);

#endif /* LIMITS_HPP */