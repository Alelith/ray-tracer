#ifndef LIMITS_HPP
# define LIMITS_HPP

# include <cmath>
# include <algorithm>

# ifdef __CUDACC__
#  define RT_CALLABLE __host__ __device__
# else
#  define RT_CALLABLE
# endif

class Limits
{
	private:
		float	min;
		float	max;
	public:
		RT_CALLABLE Limits();
		RT_CALLABLE Limits(float min, float max);
		RT_CALLABLE ~Limits() = default;

		inline RT_CALLABLE float	getMin() const { return min; }
		inline RT_CALLABLE float	getMax() const { return max; }
		inline RT_CALLABLE float	getCenter() const { return (min + max) / 2.0f; }
		inline RT_CALLABLE float	getLength() const { return max - min; }

		inline RT_CALLABLE bool		isBetween(float t) const { return t >= min && t <= max; }
		inline RT_CALLABLE float	clamp(float t) const { return fmaxf(min, fminf(max, t)); }

		inline RT_CALLABLE Limits	expand(float d) const { return Limits(min - d/2, max + d/2); }
};

inline RT_CALLABLE Limits	getInterval(const Limits &a, const Limits &b) { return Limits(fminf(a.getMin(), b.getMin()), fmaxf(a.getMax(), b.getMax())); }

#endif /* LIMITS_HPP */
