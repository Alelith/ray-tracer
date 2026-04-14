#include "Vector3.cuh"
RT_CALLABLE Vector3::Vector3() : v{0.0f, 0.0f, 0.0f} {}
RT_CALLABLE Vector3::Vector3(float x, float y, float z) : v{x, y, z} {}
std::ostream	&operator<<(std::ostream &os, const Vector3 &vec)
{
	os << "Vector3(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
	return os;
}