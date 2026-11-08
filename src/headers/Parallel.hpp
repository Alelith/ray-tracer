#ifndef PARALLEL_HPP
# define PARALLEL_HPP

# include <cstdint>

template <typename Fn>
inline void	parallelFor(uint32_t n, Fn&& fn);

# include "detail/Parallel.ipp"

#endif // PARALLEL_HPP
