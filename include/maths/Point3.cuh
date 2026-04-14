#ifndef POINT3_HPP
# define POINT3_HPP

# include "Vector3.cuh"
# include <cmath>
# include <ostream>

class Point3
{
	private:
		float	p[3];
	public:
		RT_CALLABLE Point3();
		RT_CALLABLE Point3(float x, float y, float z);
		RT_CALLABLE Point3(const Point3 &other) = default;
		RT_CALLABLE ~Point3() = default;

		RT_CALLABLE Point3	&operator=(const Point3 &other) = default;

		inline RT_CALLABLE float	&operator[](int index) { return p[index]; }
		inline RT_CALLABLE float	operator[](int index) const { return p[index]; }

		inline RT_CALLABLE float	x() const { return p[0]; }
		inline RT_CALLABLE float	y() const { return p[1]; }
		inline RT_CALLABLE float	z() const { return p[2]; }

		inline RT_CALLABLE Vector3	toVector() const { return Vector3(p[0], p[1], p[2]); }
};

std::ostream	&operator<<(std::ostream &os, const Point3 &point);

inline RT_CALLABLE Point3	operator+(const Point3 &point, const Vector3 &vec) { return Point3(point.x() + vec.x(), point.y() + vec.y(), point.z() + vec.z()); }
inline RT_CALLABLE Point3	operator+=(Point3 &point, const Vector3 &vec) { point = point + vec; return point; }
inline RT_CALLABLE Point3	operator-(const Point3 &point, const Vector3 &vec) { return Point3(point.x() - vec.x(), point.y() - vec.y(), point.z() - vec.z()); }
inline RT_CALLABLE Point3	operator-=(Point3 &point, const Vector3 &vec) { point = point - vec; return point; }
inline RT_CALLABLE Vector3	operator-(const Point3 &p1, const Point3 &p2) { return Vector3(p1.x() - p2.x(), p1.y() - p2.y(), p1.z() - p2.z()); }

inline RT_CALLABLE float	dot(const Point3 &p1, const Point3 &p2) { return p1.x() * p2.x() + p1.y() * p2.y() + p1.z() * p2.z(); }
inline RT_CALLABLE float	dot(const Point3 &point, const Vector3 &vec) { return point.x() * vec.x() + point.y() * vec.y() + point.z() * vec.z(); }
inline RT_CALLABLE float	dot(const Vector3 &vec, const Point3 &point) { return vec.x() * point.x() + vec.y() * point.y() + vec.z() * point.z(); }

#endif /* POINT3_HPP */
