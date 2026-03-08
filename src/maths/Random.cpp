#include "Random.hpp"

Random&	Random::global()
{
	std::random_device			rd;
	static Random				instance(static_cast<unsigned long>(rd()) ^
							(static_cast<unsigned long>(rd()) << 32));
	return instance;
}

void	Random::reseed(unsigned long seed)
{
	global().state = seed ? seed : 1UL;
}

float	Random::next()
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return static_cast<float>(state) / static_cast<float>(ULONG_MAX);
}

float	Random::randomfloat()
{
	return next();
}

float	Random::randomfloat(float min, float max)
{
	return min + (max - min) * next();
}

Vector3	Random::randomVector3()
{
	return Vector3(next(), next(), next());
}

Vector3	Random::randomVector3(float min, float max)
{
	return Vector3(randomfloat(min, max), randomfloat(min, max), randomfloat(min, max));
}

Vector3	Random::randomUnitVector3()
{
	float a = randomfloat(0, 2 * M_PI);
	float z = randomfloat(-1, 1);
	float r = sqrt(1.0 - z * z);
	return Vector3(r * cos(a), r * sin(a), z);
}

Vector3	Random::randomInUnitSphere()
{
	return randomUnitVector3() * cbrt(next());
}

Vector3	Random::randomInUnitDisk()
{
	float r     = sqrt(next());
	float theta = randomfloat(0, 2 * M_PI);
	return Vector3(r * cos(theta), r * sin(theta), 0);
}

Vector3	Random::randomInHemisphere(const Vector3 &normal)
{
	Vector3 v = randomUnitVector3();
	return dot(v, normal) > 0.0 ? v : -v;
}
