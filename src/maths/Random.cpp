#include "Random.hpp"

unsigned long Random::seed = static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count());

void	Random::xorshift64()
{
	seed ^= seed << 13;
	seed ^= seed >> 7;
	seed ^= seed << 17;
}

double	Random::randomDouble()
{
	xorshift64();
	return static_cast<double>(seed) / static_cast<double>(ULONG_MAX);
}

double	Random::randomDouble(double min, double max)
{
	return min + (max - min) * randomDouble();
}

Vector3	Random::randomVector3()
{
	return Vector3(randomDouble(), randomDouble(), randomDouble());
}

Vector3	Random::randomVector3(double min, double max)
{
	return Vector3(randomDouble(min, max), randomDouble(min, max), randomDouble(min, max));
}

Vector3	Random::randomUnitVector3()
{
	double a = randomDouble(0, 2 * M_PI);
	double z = randomDouble(-1, 1);
	double r = sqrt(1 - z * z);
	return Vector3(r * cos(a), r * sin(a), z);
}

Vector3	Random::randomInHemisphere(const Vector3 &normal)
{
	Vector3 inUnitSphere = randomInUnitSphere();
	if (dot(inUnitSphere, normal) > 0.0)
		return inUnitSphere;
	else
		return -inUnitSphere;
}

Vector3	Random::randomInUnitSphere()
{
	while (true)
	{
		Vector3 p = randomVector3(-1, 1);
		if (p.length_sqrd() < 1)
			return p;
	}
}

Vector3	Random::randomInUnitDisk()
{
	while (true)
	{
		Vector3 p = Vector3(randomDouble(-1, 1), randomDouble(-1, 1), 0);
		if (p.length_sqrd() < 1)
			return p;
	}
}
