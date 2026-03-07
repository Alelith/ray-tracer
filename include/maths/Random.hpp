#ifndef RANDOM_HPP
# define RANDOM_HPP

# include "Vector3.hpp"
# include <cmath>
# include <climits>

struct RNG
{
	unsigned long	state;

	explicit RNG(unsigned long seed = 123456789UL)
		: state(seed ? seed : 1UL) {}

	double			next();

	double			randomDouble();
	double			randomDouble(double min, double max);

	Vector3			randomVector3();
	Vector3			randomVector3(double min, double max);
	Vector3			randomUnitVector3();
	Vector3			randomInUnitSphere();
	Vector3			randomInUnitDisk();
	Vector3			randomInHemisphere(const Vector3 &normal);
};

#endif /* RANDOM_HPP */
