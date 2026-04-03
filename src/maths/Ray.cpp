#include "Ray.hpp"

RT_CALLABLE Ray::Ray() : origin(Point3(0, 0, 0)), direction(Vector3(0, 0, 1)) {}

RT_CALLABLE Ray::Ray(const Point3 &origin, const Vector3 &direction) : origin(origin), direction(direction) {}

std::ostream	&operator<<(std::ostream &os, const Ray &ray)
{
	os << "Ray(Origin: " << ray.getOrigin() << ", Direction: " << ray.getDirection() << ")";
	return os;
}
