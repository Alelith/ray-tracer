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

double	Random::next()
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return static_cast<double>(state) / static_cast<double>(ULONG_MAX);
}

double	Random::randomDouble()
{
	return next();
}

double	Random::randomDouble(double min, double max)
{
	return min + (max - min) * next();
}

Vector3	Random::randomVector3()
{
	return Vector3(next(), next(), next());
}

Vector3	Random::randomVector3(double min, double max)
{
	return Vector3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

Vector3	Random::randomUnitVector3()
{
	double a = randomDouble(0, 2 * M_PI);
	double z = randomDouble(-1, 1);
	double r = sqrt(1.0 - z * z);
	return Vector3(r * cos(a), r * sin(a), z);
}

Vector3	Random::randomInUnitSphere()
{
	return randomUnitVector3() * cbrt(next());
}

Vector3	Random::randomInUnitDisk()
{
	double r     = sqrt(next());
	double theta = randomDouble(0, 2 * M_PI);
	return Vector3(r * cos(theta), r * sin(theta), 0);
}

Vector3	Random::randomInHemisphere(const Vector3 &normal)
{
	Vector3 v = randomUnitVector3();
	return dot(v, normal) > 0.0 ? v : -v;
}
