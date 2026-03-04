#ifndef RANDOM_HPP
# define RANDOM_HPP

# include "Vector3.hpp"
# include <cmath>
# include <climits>

// Per-instance RNG — holds its own state so every GPU thread can have its own.
// Seed with a unique value per thread (e.g. pixel index, thread id, etc.).
// Uses xorshift64: fast, no heap, no atomics, no std::chrono — GPU-compatible.
struct RNG
{
	unsigned long	state;

	// Seed must be non-zero; passing 0 is promoted to 1.
	explicit RNG(unsigned long seed = 123456789UL)
		: state(seed ? seed : 1UL) {}

	// Advance state and return a double in [0, 1)
	double			next();

	double			randomDouble();
	double			randomDouble(double min, double max);

	Vector3			randomVector3();
	Vector3			randomVector3(double min, double max);

	// Uniform direction on the unit sphere (no rejection loop)
	Vector3			randomUnitVector3();

	// Uniform point inside the unit sphere (no rejection loop)
	Vector3			randomInUnitSphere();

	// Uniform point inside the unit disk in XY (no rejection loop)
	Vector3			randomInUnitDisk();

	Vector3			randomInHemisphere(const Vector3 &normal);
};

#endif /* RANDOM_HPP */
