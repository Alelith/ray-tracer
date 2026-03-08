#ifndef RANDOM_HPP
# define RANDOM_HPP

# include "Vector3.hpp"
# include <cmath>
# include <climits>
# include <random>

struct Random
{
	unsigned long	state;

	explicit Random(unsigned long seed = 123456789UL)
		: state(seed ? seed : 1UL) {}

	static Random&	global();
	static void		reseed(unsigned long seed);

	float			next();

	float			randomfloat();
	float			randomfloat(float min, float max);

	Vector3			randomVector3();
	Vector3			randomVector3(float min, float max);
	Vector3			randomUnitVector3();
	Vector3			randomInUnitSphere();
	Vector3			randomInUnitDisk();
	Vector3			randomInHemisphere(const Vector3 &normal);
};

#endif /* RANDOM_HPP */
