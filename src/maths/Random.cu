#include "Random.cuh"

namespace maths
{

	RT_CALLABLE float	Random::next()
	{
		state ^= state << 13;
		state ^= state >> 7;
		state ^= state << 17;
		return static_cast<float>(state) / static_cast<float>(ULONG_MAX);
	}

	RT_CALLABLE float	Random::randomfloat() { return next(); }
	RT_CALLABLE float	Random::randomfloat(float min, float max) { return min + (max - min) * next(); }

	RT_CALLABLE Vector3	Random::randomVector3() { return Vector3(next(), next(), next()); }
	RT_CALLABLE Vector3	Random::randomVector3(float min, float max) { return Vector3(randomfloat(min, max), randomfloat(min, max), randomfloat(min, max)); }
	RT_CALLABLE Vector3	Random::randomUnitVector3()
	{
		float	a = randomfloat(0.0f, 2.0f * M_PI_F);
		float	z = randomfloat(-1.0f, 1.0f);
		float	r = sqrtf(1.0f - z * z);
		return Vector3(r * cosf(a), r * sinf(a), z);
	}
	RT_CALLABLE Vector3	Random::randomInUnitSphere() { return randomUnitVector3() * cbrtf(next()); }
	RT_CALLABLE Vector3	Random::randomInUnitDisk()
	{
		float	r = sqrtf(next());
		float	theta = randomfloat(0.0f, 2.0f * M_PI_F);
		return Vector3(r * cosf(theta), r * sinf(theta), 0.0f);
	}
	RT_CALLABLE Vector3	Random::randomInHemisphere(const Vector3 &normal)
	{
		Vector3	v = randomUnitVector3();
		return dot(v, normal) > 0.0f ? v : -v;
	}

} // namespace maths
