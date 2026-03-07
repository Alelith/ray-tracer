#include "Random.hpp"

double	RNG::next()
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return static_cast<double>(state) / static_cast<double>(ULONG_MAX);
}

double	RNG::randomDouble()
{
	return next();
}

double	RNG::randomDouble(double min, double max)
{
	return min + (max - min) * next();
}

Vector3	RNG::randomVector3()
{
	return Vector3(next(), next(), next());
}

Vector3	RNG::randomVector3(double min, double max)
{
	return Vector3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

Vector3	RNG::randomUnitVector3()
{
	double a = randomDouble(0, 2 * M_PI);
	double z = randomDouble(-1, 1);
	double r = sqrt(1.0 - z * z);
	return Vector3(r * cos(a), r * sin(a), z);
}

Vector3	RNG::randomInUnitSphere()
{
	return randomUnitVector3() * cbrt(next());
}

Vector3	RNG::randomInUnitDisk()
{
	double r     = sqrt(next());
	double theta = randomDouble(0, 2 * M_PI);
	return Vector3(r * cos(theta), r * sin(theta), 0);
}

Vector3	RNG::randomInHemisphere(const Vector3 &normal)
{
	Vector3 v = randomInUnitSphere();
	return dot(v, normal) > 0.0 ? v : -v;
}
