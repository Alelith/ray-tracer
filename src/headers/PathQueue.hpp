#ifndef PATHQUEUE_HPP
# define PATHQUEUE_HPP

# include <cstdint>
# include <vector>
# include <atomic>
# include <algorithm>
# include "Ray.hpp"

class PathQueue {
	public:
		inline void		allocate(uint32_t cap_);
		inline uint32_t	capacity() const;
		inline uint32_t	size() const;
		inline void		clear();
		inline uint32_t	append();
		inline void		setSize(uint32_t n);
		inline void		appendBlock(PathQueue& block);
		inline Ray		ray(uint32_t i) const;
		inline void		setRay(uint32_t i, const Ray& r);
		inline Color	throughput(uint32_t i) const;
		inline void		setThroughput(uint32_t i, const Color& c);
		template <typename F>
		inline void		zip(PathQueue& o, F&& f);

		std::vector<float>		ox, oy, oz;
		std::vector<float>		dx, dy, dz;
		std::vector<uint32_t>	pixel;
		std::vector<uint32_t>	rng;
		std::vector<float>		tr, tg, tb;

		std::vector<uint32_t>	countEmission;

		std::vector<float>		hitT;
		std::vector<uint32_t>	hitPrim;

	private:
		std::atomic<uint32_t>	count{0};
		uint32_t				cap = 0;
};

class ShadowQueue {
	public:
		inline void		allocate(uint32_t cap_);
		inline uint32_t	size() const;
		inline void		clear();
		inline uint32_t	append();
		inline void		appendBlock(const ShadowQueue& block);

		std::vector<float>		ox, oy, oz, dx, dy, dz;
		std::vector<uint32_t>	tMax;
		std::vector<float>		cr, cg, cb;
		std::vector<uint32_t>	pixel;

	private:
		std::atomic<uint32_t>	count{0};
};

class IndexQueue {
	public:
		inline void		allocate(uint32_t capacity);
		inline uint32_t	size() const;
		inline void		clear();
		inline void		push(uint32_t pathIndex);
		inline void		appendBlock(const IndexQueue& block);

		std::vector<uint32_t>	index;

	private:
		std::atomic<uint32_t>	count{0};
};

template <typename Queue>
class Batch {
	public:
		Batch(Queue& target, uint32_t maxEntries);
		~Batch();
		Batch(const Batch&) = delete;
		Batch&		operator=(const Batch&) = delete;

		Queue		local;

	private:
		Queue&		target;
};

# include "detail/PathQueue.ipp"

#endif // PATHQUEUE_HPP
