/**
 * @file Random.cu
 * @brief Implementation of the PRNG methods
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Random.cuh"

/**
 * @brief Computes the next pseudo-random value
 * 
 * @details Generates a random flow using a simple XOR shift algorithm.
 * Mutates the internal state and returns a normalized float.
 * 
 * @ingroup math_module
 * 
 * @return A float value uniformly distributed in [0, 1]
 */
RT_CALLABLE float	Random::next()
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return static_cast<float>(state) / static_cast<float>(ULONG_MAX);
}

/**
 * @brief Wraps the next() function to provide standard random float
 * 
 * @ingroup math_module
 * 
 * @return A float value uniformly distributed in [0, 1]
 */
RT_CALLABLE float	Random::randomfloat()
{
	return next();
}

/**
 * @brief Generates a random float within a specified range
 * 
 * @ingroup math_module
 * 
 * @param[in] min Lower bound
 * @param[in] max Upper bound
 * @return A float value uniformly distributed in [min, max]
 */
RT_CALLABLE float	Random::randomfloat(float min, float max)
{
	return min + (max - min) * next();
}

/**
 * @brief Generates a random 3D vector with components in [0, 1]
 * 
 * @ingroup math_module
 * 
 * @return Vector3 with randomized components
 */
RT_CALLABLE Vector3	Random::randomVector3()
{
	return Vector3(next(), next(), next());
}

/**
 * @brief Generates a random 3D vector with components within range
 * 
 * @ingroup math_module
 * 
 * @param[in] min Minimum boundary value for components
 * @param[in] max Maximum boundary value for components
 * @return Vector3 with bounded randomized components
 */
RT_CALLABLE Vector3	Random::randomVector3(float min, float max)
{
	return Vector3(randomfloat(min, max), randomfloat(min, max), randomfloat(min, max));
}

/**
 * @brief Generates a random vector that lays on the unit sphere surface
 * 
 * @ingroup math_module
 * 
 * @return Normalized Vector3
 */
RT_CALLABLE Vector3	Random::randomUnitVector3()
{
	float a = randomfloat(0.0f, 2.0f * M_PI_F);
	float z = randomfloat(-1.0f, 1.0f);
	float r = sqrtf(1.0f - z * z);
	return Vector3(r * cosf(a), r * sinf(a), z);
}

/**
 * @brief Generates a random vector uniformly distributed within the unit sphere
 * 
 * @ingroup math_module
 * 
 * @return Vector3 whose magnitude is <= 1.0
 */
RT_CALLABLE Vector3	Random::randomInUnitSphere()
{
	return randomUnitVector3() * cbrtf(next());
}

/**
 * @brief Generates a random vector uniformly distributed within a unit disk on the XY plane
 * 
 * @ingroup math_module
 * 
 * @return Vector3 with zero Z component and length <= 1.0
 */
RT_CALLABLE Vector3	Random::randomInUnitDisk()
{
	float r     = sqrtf(next());
	float theta = randomfloat(0.0f, 2.0f * M_PI_F);
	return Vector3(r * cosf(theta), r * sinf(theta), 0.0f);
}

/**
 * @brief Generates a random unit vector distributed over a hemisphere defined by a normal
 * 
 * @ingroup math_module
 * 
 * @param[in] normal The surface normal describing the hemisphere direction
 * @return Normalized Vector3 pointing into the same hemisphere as the normal
 */
RT_CALLABLE Vector3	Random::randomInHemisphere(const Vector3 &normal)
{
	Vector3 v = randomUnitVector3();
	return dot(v, normal) > 0.0f ? v : -v;
}
