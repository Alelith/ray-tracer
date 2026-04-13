/**
 * @file Ray.cuh
 * @brief Ray class for the ray tracing engine
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef RAY_HPP
# define RAY_HPP

# include "Vector3.cuh"
# include "Point3.cuh"

/**
 * @class Ray
 * @brief Represents a geometric ray in 3D space
 * 
 * A ray consists of an origin point and a direction vector. It is used
 * fundamentally by the ray tracer to sample the scene.
 * 
 * @ingroup math_module
 */
class Ray
{
	private:
		Point3	origin;     ///< The starting point of the ray
		Vector3	direction;  ///< The direction vector of the ray
	public:
		RT_CALLABLE Ray();
		RT_CALLABLE Ray(const Point3 &origin, const Vector3 &direction);
		RT_CALLABLE Ray(const Ray &other) = default;
		RT_CALLABLE ~Ray() = default;

		inline RT_CALLABLE Point3	getOrigin() const { return origin; }
		inline RT_CALLABLE Vector3	getDirection() const { return direction; }

		/**
		 * @brief Gets the point along the ray at parameter t
		 * 
		 * @param[in] t The scalar parameter along the ray direction
		 * @return The point evaluated at origin + t * direction
		 */
		inline RT_CALLABLE Point3	at(float t) const { return origin + direction * t; }
};

std::ostream	&operator<<(std::ostream &os, const Ray &ray);

#endif /* RAY_HPP */
