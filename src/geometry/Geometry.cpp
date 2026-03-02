#include "Geometry.hpp"

Limits::Limits(double tMin = 0.001, double tMax = INFINITY) : tMin(tMin), tMax(tMax) {}

bool	Limits::isBetween(double n)
{ return n >= tMin && n <= tMax; }

Ray	Geometry::getLocalRay(const Ray &ray) const
{
	return (Ray(transform.getInverseMatrix() * ray.getOrigin(), transform.getInverseMatrix() * ray.getDirection()));
}
