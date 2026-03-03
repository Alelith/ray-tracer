#include "Geometry.hpp"

Ray	Geometry::getLocalRay(const Ray &ray) const
{
	return (Ray(transform.getInverseMatrix() * ray.getOrigin(), transform.getInverseMatrix() * ray.getDirection()));
}
