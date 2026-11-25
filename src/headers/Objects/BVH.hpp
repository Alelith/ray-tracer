#ifndef BVH_HPP
# define BVH_HPP

# include <vector>
# include "Objects/Geometry.hpp"

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
		inline static float	area(const AABB& n);
		inline uint32_t		buildRecursive(std::vector<uint32_t>& order, uint32_t begin, uint32_t end);

		std::vector<BVHNode>	nodes;
		std::vector<AABB>		boxes;
		std::vector<Vec3>		centroids;
};

# include "detail/Objects/BVH.ipp"

#endif // BVH_HPP
