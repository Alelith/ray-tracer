#ifndef AABB_HPP
# define AABB_HPP

#include "Ray.hpp"
#include "Limits.hpp"
#include "Transform.hpp"

class AABB
{
	private:
		Limits	xLimits;
		Limits	yLimits;
		Limits	zLimits;
	public:
		AABB(const Limits &xLimits, const Limits &yLimits, const Limits &zLimits);
		AABB(const Point3 &a, const Point3 &b);
		AABB(const AABB &box0, const AABB &box1);
		AABB(const Point3 &localMin, const Point3 &localMax, const Transform &transform);
		~AABB();

		const Limits	&getXLimits() const;
		const Limits	&getYLimits() const;
		const Limits	&getZLimits() const;

		bool			hit(const Ray &ray, const Limits &limits) const;
};

#endif /* AABB_HPP */
