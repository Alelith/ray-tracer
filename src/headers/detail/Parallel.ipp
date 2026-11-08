template <typename Fn>
inline void parallelFor(uint32_t n, Fn&& fn) { for (uint32_t i = 0; i < n; i++) fn(i); }
