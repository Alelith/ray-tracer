#include "Point3.hpp"
#include "Vector3.hpp"

RT_CALLABLE Point3::Point3() : p{0.0, 0.0, 0.0} {}

RT_CALLABLE Point3::Point3(float x, float y, float z) : p{x, y, z} {}

const Point3	Point3::Origin(0.0, 0.0, 0.0);

std::ostream	&operator<<(std::ostream &os, const Point3 &point)
{
	os << "Point3(" << point[0] << ", " << point[1] << ", " << point[2] << ")";
	return os;
}
