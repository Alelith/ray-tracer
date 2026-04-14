#ifndef AABB_HPP
# define AABB_HPP

# include "Ray.cuh"
# include "Limits.cuh"
# include "Matrix4x4.cuh"

namespace geometry
{

	using namespace maths;

	class AABB
	{
		private:
			Limits	xLimits;
			Limits	yLimits;
			Limits	zLimits;
		public:
			RT_CALLABLE AABB();
			RT_CALLABLE AABB(const Limits &xLimits, const Limits &yLimits, const Limits &zLimits);
			RT_CALLABLE AABB(const Point3 &a, const Point3 &b);
			RT_CALLABLE AABB(const AABB &box0, const AABB &box1);
			RT_CALLABLE AABB(const Point3 &localMin, const Point3 &localMax, const Matrix4x4 &masterMat);
			RT_CALLABLE ~AABB() = default;

			const RT_CALLABLE Limits	&getXLimits() const;
			const RT_CALLABLE Limits	&getYLimits() const;
			const RT_CALLABLE Limits	&getZLimits() const;

			RT_CALLABLE float	getArea() const;
			RT_CALLABLE bool	hit(const Ray &ray, const Limits &limits) const;
	};

} // namespace geometry

#endif /* AABB_HPP */
