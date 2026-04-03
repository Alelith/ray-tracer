#ifndef POINT3_HPP
# define POINT3_HPP

# include <cmath>
# include <ostream>

# include "Vector3.hpp"

class Point3
{
	private:
		float	p[3];
	public:
		Point3();
		Point3(float x, float y, float z);
		Point3(const Point3 &other) = default;
		~Point3() = default;

		static const	Point3 Origin;

		Point3			&operator=(const Point3 &other) = default;

		inline float			&operator[](int index) { return p[index]; }
		inline float			operator[](int index) const { return p[index]; }

		inline float			x() const { return p[0]; }
		inline float			y() const { return p[1]; }
		inline float			z() const { return p[2]; }

		// Convert to vector (position vector from origin)
		inline Vector3			toVector() const { return Vector3(p[0], p[1], p[2]); }
};

std::ostream	&operator<<(std::ostream &os, const Point3 &point);

// Point + Vector = Point
inline Point3			operator+(const Point3 &point, const Vector3 &vec)
{
	return Point3(point.x() + vec.x(), point.y() + vec.y(), point.z() + vec.z());
}

inline Point3			operator+=(Point3 &point, const Vector3 &vec)
{
	point = point + vec;
	return point;
}

// Point - Vector = Point
inline Point3			operator-(const Point3 &point, const Vector3 &vec)
{
	return Point3(point.x() - vec.x(), point.y() - vec.y(), point.z() - vec.z());
}

inline Point3			operator-=(Point3 &point, const Vector3 &vec)
{
	point = point - vec;
	return point;
}

// Point - Point = Vector
inline Vector3			operator-(const Point3 &p1, const Point3 &p2)
{
	return Vector3(p1.x() - p2.x(), p1.y() - p2.y(), p1.z() - p2.z());
}

#endif /* POINT3_HPP */
