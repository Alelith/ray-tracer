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

		inline float	getMin() const { return min; }
		inline float	getMax() const { return max; }

		inline float	getCenter() const { return (min + max) / 2.0f; }
		inline float	getLength() const { return max - min; }

		inline bool		isBetween(float t) const { return t >= min && t <= max; }
		inline float	clamp(float t) const { return std::max(min, std::min(max, t)); }
		inline Limits	expand(float delta) const { return Limits(min - delta, max + delta); }
};

inline Limits	getInterval(const Limits &a, const Limits &b) { return Limits(std::max(a.getMin(), b.getMin()), std::min(a.getMax(), b.getMax())); }

#endif /* LIMITS_HPP */