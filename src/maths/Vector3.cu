/**
 * @file Vector3.cu
 * @brief Implementation of the 3D Vector class
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Vector3.cuh"

/**
 * @brief Default constructor initializing to zero components
 * 
 * @ingroup math_module
 */
RT_CALLABLE Vector3::Vector3() : v{0.0f, 0.0f, 0.0f} {}

/**
 * @brief Parameterized constructor
 * 
 * @ingroup math_module
 * 
 * @param[in] x The X component
 * @param[in] y The Y component
 * @param[in] z The Z component
 */
RT_CALLABLE Vector3::Vector3(float x, float y, float z) : v{x, y, z} {}

/**
 * @brief Streaming operator for text output
 * 
 * @ingroup math_module
 * 
 * @param[in,out] os Standard stream
 * @param[in] vec The vector to format
 * @return stream modified stream
 */
std::ostream	&operator<<(std::ostream &os, const Vector3 &vec)
{
	os << "Vector3(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
	return os;
}
