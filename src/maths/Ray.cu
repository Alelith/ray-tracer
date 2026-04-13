/**
 * @file Ray.cu
 * @brief Implementation of the Ray class
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Ray.cuh"

/**
 * @brief Default constructor initializing an origin ray facing positive Z
 * 
 * @ingroup math_module
 */
RT_CALLABLE Ray::Ray() : origin(Point3(0, 0, 0)), direction(Vector3(0, 0, 1)) {}

/**
 * @brief Parameterized constructor
 * 
 * @ingroup math_module
 * 
 * @param[in] origin Spawning position
 * @param[in] direction Direction vector
 */
RT_CALLABLE Ray::Ray(const Point3 &origin, const Vector3 &direction) : origin(origin), direction(direction) {}

/**
 * @brief Streaming operator for text output
 * 
 * @ingroup math_module
 * 
 * @param[in,out] os Standard stream
 * @param[in] ray The ray
 * @return stream modified stream
 */
std::ostream	&operator<<(std::ostream &os, const Ray &ray)
{
	os << "Ray(Origin: " << ray.getOrigin() << ", Direction: " << ray.getDirection() << ")";
	return os;
}
