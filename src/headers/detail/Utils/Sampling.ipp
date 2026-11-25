#include "Maths/RNG.hpp"

inline Vec3 randomInUnitSphere(uint32_t& rng) {
	while (true) {
		Vec3 p{2.0f * rngNext(rng) - 1.0f, 2.0f * rngNext(rng), 2.0f * rngNext(rng) };
		if (p.lengthSquared() < 1.0f) return p;
	}
}
inline Vec3 randomUnitVector(uint32_t& rng) {
	float z = 2.0f * rngNext(rng) - 1.0f;
	float phi = 2.0f * KPI * rngNext(rng);
	float r = std::sqrt(std::fmax(0.0f, 1.0f - z * z));

	return {r * std::cos(phi), r * std::sin(phi), z};
}
inline Vec3		randomInUnitDisk(uint32_t& rng) {
	float r = std::sqrt(rngNext(rng));
	float phi = 2.0f * KPI * rngNext(rng);

	return {r * std::cos(phi), r * std::sin(phi), 0.0f};
}
inline void buildBasis(const Vec3& n, Vec3& u, Vec3& v) {
	float sign = std::copysign(1.0f, n.z);
	float a = -1.0f / (sign + n.z);
	float b = n.x * n.y * a;
	u = Vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
	v = Vec3(b, sign + n.y * n.y * a, -n.y);
}
