#include "Geometry.hpp"

Ray	getLocalRay(const Transform &transform, const Ray &ray)
{
	return Ray(transform.getInverseMatrix() * ray.getOrigin(),
			   transform.getInverseMatrix() * ray.getDirection());
}

bool	geometryHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
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
}
