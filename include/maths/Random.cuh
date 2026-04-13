/**
 * @file Random.cuh
 * @brief Random number and vector generator for Monte Carlo sampling
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef RANDOM_HPP
# define RANDOM_HPP

# include "Vector3.cuh"
# include <climits>

/**
 * @struct Random
 * @brief Simple pseudorandom number generator (PRNG) state and methods
 * 
 * Generates uniformly distributed pseudo-random numbers and random vectors,
 * primarily used for path tracing algorithms requiring ray scattering
 * and stochastic sampling.
 * 
 * @ingroup math_module
 */
struct Random
{
	unsigned long	state; ///< Internal state of the PRNG

	/**
	 * @brief Constructs the generator with an initial seed
	 * @param[in] seed Initial deterministic seed
	 */
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
