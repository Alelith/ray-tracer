#ifndef PATHQUEUE_HPP
# define PATHQUEUE_HPP

# include <cstdint>
# include <vector>
# include <atomic>
# include "Ray.hpp"

class PathQueue {
	public:
		inline void		allocate(uint32_t cap_);
		inline uint32_t	capacity() const;
		inline uint32_t	size() const;
		inline void		clear();
		inline uint32_t	append();
		inline Ray		ray(uint32_t i) const;
		inline void		setRay(uint32_t i, const Ray& r);

		std::vector<float>		ox, oy, oz;
		std::vector<float>		dx, dy, dz;
		std::vector<uint32_t>	pixel;

	private:
		std::atomic<uint32_t>	count{0};
		uint32_t				cap = 0;
};

# include "detail/PathQueue.ipp"

#endif // PATHQUEUE_HPP
