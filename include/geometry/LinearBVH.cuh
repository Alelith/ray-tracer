#ifndef LINEARBVH_HPP
# define LINEARBVH_HPP

# include "Geometry.cuh"
# include <vector>

namespace geometry
{

	using std::vector;

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
			WorldGeometries			worldGeoms;

			RT_CALLABLE int	buildNode(vector<BVHData> &data, int start, int end);
			RT_CALLABLE int	bestSplit(vector<BVHData> &data, int start, int end);
		public:
			RT_CALLABLE LinearBVH() : bvhData(nullptr), count(0) {}
			RT_CALLABLE LinearBVH(const WorldGeometries &worldGeoms);
			RT_CALLABLE ~LinearBVH() = default;

			RT_CALLABLE bool	hit(const Ray &ray, const Limits &limits, HitRecord &record) const;

			LinearBVH	cudaCopy() const;
			void		cudaFreeBVH();
	};

} // namespace geometry

#endif /* LINEARBVH_HPP */
