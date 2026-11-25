#ifndef SAMPLING_HPP
# define SAMPLING_HPP

# include <cstdint>
# include "Maths/Vec3.hpp"

inline Vec3		randomInUnitSphere(uint32_t& rng);
inline Vec3		randomUnitVector(uint32_t& rng);
inline Vec3		randomInUnitDisk(uint32_t& rng);
inline void		buildBasis(const Vec3& n, Vec3& u, Vec3& v);

# include "detail/Utils/Sampling.ipp"

#endif // SAMPLING_HPP
