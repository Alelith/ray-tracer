#ifndef RNG_HPP
# define RNG_HPP

# include <cstdint>

inline uint32_t		rngNextU32(uint32_t& state);
inline float		rngNext(uint32_t& state);
inline uint32_t		hash32(uint32_t v);
inline uint32_t		makeSeed(uint32_t pixel, uint32_t sample);

# include "detail/RNG.ipp"

#endif // RNG_HPP
