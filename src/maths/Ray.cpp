#include "Ray.hpp"

Ray::Ray() : origin(Point3::Zero), direction(Vector3::Forward) {}

Ray::Ray(const Point3 &origin, const Vector3 &direction) : origin(origin), direction(direction) {}



Point3	Ray::getOrigin() const { return origin; }

Vector3	Ray::getDirection() const { return direction; }

Point3	Ray::at(double t) const { return origin + t * direction; }

std::ostream	&operator<<(std::ostream &os, const Ray &ray)
{
	os << "Ray(Origin: " << ray.getOrigin() << ", Direction: " << ray.getDirection() << ")";
	return os;
}
