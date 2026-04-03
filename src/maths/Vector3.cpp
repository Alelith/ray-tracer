#include "Vector3.hpp"

RT_CALLABLE Vector3::Vector3() : v{0.0, 0.0, 0.0} {}

RT_CALLABLE Vector3::Vector3(float x, float y, float z) : v{x, y, z} {}

/*const Vector3	Vector3::Zero(0.0, 0.0, 0.0);
const Vector3	Vector3::Up(0.0, 1.0, 0.0);
const Vector3	Vector3::Right(1.0, 0.0, 0.0);
const Vector3	Vector3::Forward(0.0, 0.0, 1.0);
const Vector3	Vector3::Down(0.0, -1.0, 0.0);
const Vector3	Vector3::Left(-1.0, 0.0, 0.0);
const Vector3	Vector3::Backward(0.0, 0.0, -1.0);
const Vector3	Vector3::One(1.0, 1.0, 1.0);

const Vector3	Vector3::White(1.0, 1.0, 1.0);
const Vector3	Vector3::Black(0.0, 0.0, 0.0);
const Vector3	Vector3::Gray(0.5, 0.5, 0.5);
const Vector3	Vector3::LightGray(0.75, 0.75, 0.75);
const Vector3	Vector3::DarkGray(0.25, 0.25, 0.25);
const Vector3	Vector3::Red(1.0, 0.0, 0.0);
const Vector3	Vector3::Green(0.0, 1.0, 0.0);
const Vector3	Vector3::Blue(0.0, 0.0, 1.0);
const Vector3	Vector3::Yellow(1.0, 1.0, 0.0);
const Vector3	Vector3::Cyan(0.0, 1.0, 1.0);
const Vector3	Vector3::Magenta(1.0, 0.0, 1.0);*/

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec)
{
	os << "Vector3(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
	return os;
}
