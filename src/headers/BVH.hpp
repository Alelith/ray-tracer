#ifndef BVH_HPP
# define BVH_HPP

# include <cstdint>
# include <vector>
# include "Geometry.hpp"

struct BVHNode {
	Vec3		bmin, bmax;
	uint32_t	firstOrRight;
	uint16_t	count;
	uint16_t	axis;
};

class BVH {
	public:
		inline void		build(std::vector<Primitive>& prims);
		inline bool		closestHit(const std::vector<Primitive>& prims, const Ray& ray, float tMin, float tMax, float& tHit, uint32_t& primHit) const;
		inline bool		anyHit(const std::vector<Primitive>& prims, const Ray& ray, float tMin, float tMax) const;
		inline size_t	nodeCount() const;
	
	private:
		inline static bool	hitBox(const BVHNode& n, const Point3& o, const Vec3& invDir, float tMax);
		inline uint32_t		buildRecursive(std::vector<Primitive>& prims, uint32_t begin, uint32_t end);

		std::vector<BVHNode>	nodes;
};

# include "detail/BVH.ipp"

#endif // BVH_HPP
