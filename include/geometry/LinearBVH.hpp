#ifndef LINEARBVH_HPP
# define LINEARBVH_HPP

# include "Geometry.hpp"
# include <vector>

using std::vector;

enum GeomType
{
	SPHERE,
	PLANE,
	CYLINDER,
	CONE,
	HYPERBOLOID,
	TORUS,
	KISS,
	TRIANGLE,
	NONE
};

struct BVHData
{
	int			geomIdx;
	GeomType	type;
	int			rightChildIdx;
	AABB		box;
};

class LinearBVH
{
	private:
		BVHData					*bvhData;
		size_t					count;
		const WorldGeometries	&worldGeoms;

		RT_CALLABLE int	buildNode(vector<BVHData> &data, int start, int end);

		RT_CALLABLE int	bestSplit(vector<BVHData> &data, int start, int end);
	public:
		RT_CALLABLE LinearBVH(const WorldGeometries &worldGeoms);
		RT_CALLABLE ~LinearBVH() = default;

		RT_CALLABLE bool	hit(const Ray &ray, const Limits &limits, HitRecord &record) const;
};

#endif /* LINEARBVH_HPP */
