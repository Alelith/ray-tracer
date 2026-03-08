#include "Point3.hpp"
#include "Vector3.hpp"

Point3::Point3() : p{0.0, 0.0, 0.0} {}

Point3::Point3(double x, double y, double z) : p{x, y, z} {}

const Point3	Point3::Origin(0.0, 0.0, 0.0);

double	&Point3::operator[](int index)
{ return p[index]; }

double	Point3::operator[](int index) const
{ return p[index]; }

double	Point3::x() const
{ return p[0]; }

double	Point3::y() const
{ return p[1]; }

double	Point3::z() const
{ return p[2]; }

Vector3	Point3::toVector() const
{ return Vector3(p[0], p[1], p[2]); }

std::ostream	&operator<<(std::ostream &os, const Point3 &point)
{
	os << "Point3(" << point[0] << ", " << point[1] << ", " << point[2] << ")";
	return os;
}

// Point + Vector = Point
Point3	operator+(const Point3 &point, const Vector3 &vec)
{
	return Point3(point[0] + vec[0], point[1] + vec[1], point[2] + vec[2]);
}

Point3	operator+=(Point3 &point, const Vector3 &vec)
{
	point[0] += vec[0];
	point[1] += vec[1];
	point[2] += vec[2];
	return point;
}

// Point - Vector = Point
Point3	operator-(const Point3 &point, const Vector3 &vec)
{
	return Point3(point[0] - vec[0], point[1] - vec[1], point[2] - vec[2]);
}

Point3	operator-=(Point3 &point, const Vector3 &vec)
{
	point[0] -= vec[0];
	point[1] -= vec[1];
	point[2] -= vec[2];
	return point;
}

// Point - Point = Vector
Vector3	operator-(const Point3 &p1, const Point3 &p2)
{
	return Vector3(p1[0] - p2[0], p1[1] - p2[1], p1[2] - p2[2]);
}
