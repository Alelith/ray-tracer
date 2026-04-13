/**
 * @file Point3.cu
 * @brief Implementation of the 3D Point class
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Point3.cuh"
#include "Vector3.cuh"

/**
 * @brief Default constructor initializing to the origin (0,0,0)
 * 
 * @ingroup math_module
 */
RT_CALLABLE Point3::Point3() : p{0.0, 0.0, 0.0} {}

/**
 * @brief Parameterized constructor
 * 
 * @ingroup math_module
 * 
 * @param[in] x The X coordinate
 * @param[in] y The Y coordinate
 * @param[in] z The Z coordinate
 */
RT_CALLABLE Point3::Point3(float x, float y, float z) : p{x, y, z} {}

/**
 * @brief Streaming operator for text output
 * 
 * @ingroup math_module
 * 
 * @param[in,out] os Standard output stream
 * Point to log
 * @param[in] point The point
 * @return stream modified stream
 */
std::ostream	&operator<<(std::ostream &os, const Point3 &point)
{
	os << "Point3(" << point[0] << ", " << point[1] << ", " << point[2] << ")";
	return os;
}
