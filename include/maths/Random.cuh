#ifndef RANDOM_HPP
# define RANDOM_HPP

# include "Vector3.cuh"
# include <climits>

struct Random
{
	unsigned long	state;

	RT_CALLABLE Random(unsigned long seed = 123456789UL)
		: state(seed ? seed : 1UL) {}

	RT_CALLABLE float			next();

	RT_CALLABLE float			randomfloat();
	RT_CALLABLE float			randomfloat(float min, float max);

	RT_CALLABLE Vector3			randomVector3();
	RT_CALLABLE Vector3			randomVector3(float min, float max);
	RT_CALLABLE Vector3			randomUnitVector3();
	RT_CALLABLE Vector3			randomInUnitSphere();
	RT_CALLABLE Vector3			randomInUnitDisk();
	RT_CALLABLE Vector3			randomInHemisphere(const Vector3 &normal);
};

#endif /* RANDOM_HPP */
