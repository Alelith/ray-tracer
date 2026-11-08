#ifndef SAMPLING_HPP
# define SAMPLING_HPP

# include <cstdint>
# include "Vec3.hpp"

inline Vec3		randomInUnitSphere(uint32_t& rng);
inline Vec3		randomUnitVector(uint32_t& rng);
inline Vec3		randomInUnitDisk(uint32_t& rng);

# include "detail/Sampling.ipp"

#endif // SAMPLING_HPP
