#ifndef LINEARBVH_HPP
# define LINEARBVH_HPP

# include "Geometry.hpp"

# include <stack>

using std::stack;

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
		vector<BVHData>	bvhData;
		const WorldGeometries &worldGeoms;

		int	buildNode(vector<BVHData> &data, int start, int end);

		int	bestSplit(const vector<BVHData> &data, int start, int end);
	public:
		LinearBVH(const WorldGeometries &worldGeoms);
		~LinearBVH() = default;

		bool	hit(const Ray &ray, const Limits &limits, HitRecord &record) const;
};

#endif /* LINEARBVH_HPP */
