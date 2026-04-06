#include "Geometry.hpp"

RT_CALLABLE size_t	WorldGeometries::totalCount() const
{
	return spheres.count + planes.count + cylinders.count + cones.count +
		   hyperboloids.count + tori.count + kisses.count + triangles.count;
}

RT_CALLABLE Ray	getLocalRay(const Matrix4x4 &inverse, const Ray &ray)
{
	const Point3	&wo = ray.getOrigin();
	return Ray(inverse * wo,
			   inverse * ray.getDirection());
}
