/**
 * @file Point3.cuh
 * @brief 3D Point class for geometry and spatial representation
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef POINT3_HPP
# define POINT3_HPP

# include <cmath>
# include <ostream>

# include "Vector3.cuh"

/**
 * @defgroup math_module Mathematics Module
 * @brief Core mathematical classes and functions
 * 
 * @details This module provides fundamental mathematical structures like vectors,
 * points, matrices, and various equation solvers used extensively in ray tracing
 * and geometric calculations.
 */

/**
 * @class Point3
 * @brief Represents a point in 3D Euclidean space
 * 
 * This class stores the x, y, and z coordinates of a spatial point and
 * provides operators for point-vector and point-point operations.
 * 
 * @ingroup math_module
 */
class Point3
{
	private:
		float	p[3]; ///< Array storing x, y, and z coordinates
	public:
		RT_CALLABLE Point3();
		RT_CALLABLE Point3(float x, float y, float z);
		RT_CALLABLE Point3(const Point3 &other) = default;
		RT_CALLABLE ~Point3() = default;

		RT_CALLABLE Point3			&operator=(const Point3 &other) = default;

		inline RT_CALLABLE float	&operator[](int index) { return p[index]; }
		inline RT_CALLABLE float	operator[](int index) const { return p[index]; }

		inline RT_CALLABLE float	x() const { return p[0]; }
		inline RT_CALLABLE float	y() const { return p[1]; }
		inline RT_CALLABLE float	z() const { return p[2]; }

		/**
		 * @brief Converts the point to a position vector from the origin
		 * @return Vector3 equivalent to this point
		 */
		inline RT_CALLABLE Vector3	toVector() const { return Vector3(p[0], p[1], p[2]); }
};

std::ostream	&operator<<(std::ostream &os, const Point3 &point);

/**
 * @brief Adds a vector to a point
 * 
 * @ingroup math_module
 * 
 * @param[in] point Base point
 * @param[in] vec Translation vector
 * @return New displaced point
 */
inline RT_CALLABLE Point3	operator+(const Point3 &point, const Vector3 &vec)
{
	return Point3(point.x() + vec.x(), point.y() + vec.y(), point.z() + vec.z());
}

/**
 * @brief Adds a vector to this point in-place
 * 
 * @ingroup math_module
 * 
 * @param[in,out] point Point to translate
 * @param[in] vec Translation vector
 * @return Reference to the modified point
 */
inline RT_CALLABLE Point3	operator+=(Point3 &point, const Vector3 &vec)
{
	point = point + vec;
	return point;
}

/**
 * @brief Subtracts a vector from a point
 * 
 * @ingroup math_module
 * 
 * @param[in] point Base point
 * @param[in] vec Translation vector
 * @return New displaced point
 */
inline RT_CALLABLE Point3	operator-(const Point3 &point, const Vector3 &vec)
{
	return Point3(point.x() - vec.x(), point.y() - vec.y(), point.z() - vec.z());
}

/**
 * @brief Subtracts a vector from this point in-place
 * 
 * @ingroup math_module
 * 
 * @param[in,out] point Point to translate
 * @param[in] vec Translation vector
 * @return Reference to the modified point
 */
inline RT_CALLABLE Point3	operator-=(Point3 &point, const Vector3 &vec)
{
	point = point - vec;
	return point;
}

/**
 * @brief Calculates the displacement vector between two points
 * 
 * @ingroup math_module
 * 
 * @param[in] p1 Destination point
 * @param[in] p2 Source point
 * @return Vector representing displacement from p2 to p1
 */
inline RT_CALLABLE Vector3	operator-(const Point3 &p1, const Point3 &p2)
{
	return Vector3(p1.x() - p2.x(), p1.y() - p2.y(), p1.z() - p2.z());
}

/**
 * @brief Calculates dot product using components of two points
 * 
 * @ingroup math_module
 * 
 * @param[in] p1 First point
 * @param[in] p2 Second point
 * @return Result of the dot product calculation
 */
inline RT_CALLABLE float	dot(const Point3 &p1, const Point3 &p2) { return p1.x() * p2.x() + p1.y() * p2.y() + p1.z() * p2.z(); }

/**
 * @brief Calculates dot product of point coordinates with a vector
 * 
 * @ingroup math_module
 * 
 * @param[in] point The point
 * @param[in] vec The vector
 * @return Result of the dot product calculation
 */
inline RT_CALLABLE float	dot(const Point3 &point, const Vector3 &vec) { return point.x() * vec.x() + point.y() * vec.y() + point.z() * vec.z(); }

/**
 * @brief Calculates dot product of a vector with point coordinates
 * 
 * @ingroup math_module
 * 
 * @param[in] vec The vector
 * @param[in] point The point
 * @return Result of the dot product calculation
 */
inline RT_CALLABLE float	dot(const Vector3 &vec, const Point3 &point) { return vec.x() * point.x() + vec.y() * point.y() + vec.z() * point.z(); }

#endif /* POINT3_HPP */
