#include "Geometry.hpp"

Ray	getLocalRay(const Matrix4x4 &inverse, const Ray &ray)
{
	const Point3	&wo = ray.getOrigin();
	return Ray(inverse * wo,
			   inverse * ray.getDirection());
}

bool	geometryHit(const WorldGeometries &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	HitRecord	tmpRecord;
	bool		hitAnything = false;
	Limits		localLimits = limits;

	if (spheresHit(g.spheres, ray, localLimits, tmpRecord))
	{
		hitAnything = true;
		record = tmpRecord;
	}
	return hitAnything;
}

/*bool	geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	switch (g.type)
	{
		case GeometryType::Sphere:      return sphereHit(g, ray, limits, record);
		case GeometryType::Plane:       return planeHit(g, ray, limits, record);
		case GeometryType::Cylinder:    return cylinderHit(g, ray, limits, record);
		case GeometryType::Cone:        return coneHit(g, ray, limits, record);
		case GeometryType::Hyperboloid: return hyperboloidHit(g, ray, limits, record);
		case GeometryType::Torus:       return torusHit(g, ray, limits, record);
		case GeometryType::Kiss:        return kissHit(g, ray, limits, record);
		case GeometryType::Triangle:    return triangleHit(g, ray, limits, record);
		default:                        return false;
	}
}*/
