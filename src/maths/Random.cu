#include "Random.cuh"

RT_CALLABLE float	Random::next()
{
	state ^= state << 13;
	state ^= state >> 7;
	state ^= state << 17;
	return static_cast<float>(state) / static_cast<float>(ULONG_MAX);
}

RT_CALLABLE float	Random::randomfloat()
{
	return next();
}

RT_CALLABLE float	Random::randomfloat(float min, float max)
{
	return min + (max - min) * next();
}

RT_CALLABLE Vector3	Random::randomVector3()
{
	return Vector3(next(), next(), next());
}

RT_CALLABLE Vector3	Random::randomVector3(float min, float max)
{
	return Vector3(randomfloat(min, max), randomfloat(min, max), randomfloat(min, max));
}

RT_CALLABLE Vector3	Random::randomUnitVector3()
{
	float a = randomfloat(0, 2 * M_PI);
	float z = randomfloat(-1, 1);
	float r = sqrt(1.0 - z * z);
	return Vector3(r * cos(a), r * sin(a), z);
}

RT_CALLABLE Vector3	Random::randomInUnitSphere()
{
	return randomUnitVector3() * cbrt(next());
}

RT_CALLABLE Vector3	Random::randomInUnitDisk()
{
	float r     = sqrt(next());
	float theta = randomfloat(0, 2 * M_PI);
	return Vector3(r * cos(theta), r * sin(theta), 0);
}

RT_CALLABLE Vector3	Random::randomInHemisphere(const Vector3 &normal)
{
	Vector3 v = randomUnitVector3();
	return dot(v, normal) > 0.0 ? v : -v;
}
