#include "RNG.hpp"

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
