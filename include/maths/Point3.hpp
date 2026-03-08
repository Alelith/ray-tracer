#ifndef POINT3_HPP
# define POINT3_HPP

# include <cmath>
# include <ostream>

class Vector3;

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

		float			&operator[](int index);
		float			operator[](int index) const;

		float			x() const;
		float			y() const;
		float			z() const;

		// Convert to vector (position vector from origin)
		Vector3			toVector() const;
};

std::ostream	&operator<<(std::ostream &os, const Point3 &point);

// Point + Vector = Point
Point3			operator+(const Point3 &point, const Vector3 &vec);
Point3			operator+=(Point3 &point, const Vector3 &vec);

// Point - Vector = Point
Point3			operator-(const Point3 &point, const Vector3 &vec);
Point3			operator-=(Point3 &point, const Vector3 &vec);

// Point - Point = Vector
Vector3			operator-(const Point3 &p1, const Point3 &p2);

#endif /* POINT3_HPP */
