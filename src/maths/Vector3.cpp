#include "Vector3.hpp"

Vector3::Vector3() : v{0.0, 0.0, 0.0} {}

Vector3::Vector3(double x, double y, double z, double w) : v{x, y, z, w} {}



const Vector3	Vector3::Zero(0.0, 0.0, 0.0);
const Vector3	Vector3::Up(0.0, 1.0, 0.0);
const Vector3	Vector3::Right(1.0, 0.0, 0.0);
const Vector3	Vector3::Forward(0.0, 0.0, 1.0);
const Vector3	Vector3::Down(0.0, -1.0, 0.0);
const Vector3	Vector3::Left(-1.0, 0.0, 0.0);
const Vector3	Vector3::Backward(0.0, 0.0, -1.0);
const Vector3	Vector3::One(1.0, 1.0, 1.0);

Vector3	Vector3::operator-() const
{ return Vector3(-v[0], -v[1], -v[2]); }

double	&Vector3::operator[](int index)
{ return v[index]; }

double	Vector3::operator[](int index) const
{ return v[index]; }

Vector3	Vector3::operator+=(const Vector3 &other)
{
	v[0] += other.v[0];
	v[1] += other.v[1];
	v[2] += other.v[2];
	return *this;
}

Vector3	Vector3::operator-=(const Vector3 &other)
{
	v[0] -= other.v[0];
	v[1] -= other.v[1];
	v[2] -= other.v[2];
	return *this;
}

Vector3	Vector3::operator*=(const Vector3 &other)
{
	v[0] *= other.v[0];
	v[1] *= other.v[1];
	v[2] *= other.v[2];
	return *this;
}

Vector3	Vector3::operator*=(double scalar)
{
	v[0] *= scalar;
	v[1] *= scalar;
	v[2] *= scalar;
	return *this;
}

Vector3	Vector3::operator/=(double scalar)
{
	v[0] /= scalar;
	v[1] /= scalar;
	v[2] /= scalar;
	return *this;
}

double	Vector3::length_sqrd() const
{ return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }

double	Vector3::length() const
{ return std::sqrt(length_sqrd()); }

Vector3	Vector3::normalized() const
{
	double len = length();
	if (len == 0.0)
		return Vector3::Zero;
	return *this / len;
}

bool	Vector3::isNearZero() const
{
	const double s = 1e-8;
	return (std::fabs(v[0]) < s) && (std::fabs(v[1]) < s) && (std::fabs(v[2]) < s);
}

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec)
{
	os << "Vector3(" << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
	return os;
}

Vector3	operator+(const Vector3 &vec1, const Vector3 &vec2)
{ return Vector3(vec1) += vec2; }

Vector3	operator-(const Vector3 &vec1, const Vector3 &vec2)
{ return Vector3(vec1) -= vec2; }

Vector3	operator*(const Vector3 &vec1, const Vector3 &vec2)
{ return Vector3(vec1) *= vec2; }

Vector3	operator*(double scalar, const Vector3 &vec)
{ return Vector3(vec) *= scalar; }

Vector3	operator*(const Vector3 &vec, double scalar)
{ return Vector3(vec) *= scalar; }

Vector3	operator/(const Vector3 &vec, double scalar)
{ return Vector3(vec) /= scalar; }

double	dot(const Vector3 &vec1, const Vector3 &vec2)
{ return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]; }

Vector3	cross(const Vector3 &vec1, const Vector3 &vec2)
{
	return Vector3(
		vec1[1] * vec2[2] - vec1[2] * vec2[1],
		vec1[2] * vec2[0] - vec1[0] * vec2[2],
		vec1[0] * vec2[1] - vec1[1] * vec2[0]
	);
}

Vector3	reflect(const Vector3 &vec, const Vector3 &normal)
{
	return vec - 2 * dot(vec, normal) * normal;
}

Vector3	refract(const Vector3 &uv, const Vector3 &normal, double eta_over_eta_prime)
{
	double cos_theta = std::fmin(dot(-uv, normal), 1.0);
	Vector3 r_out_perp = eta_over_eta_prime * (uv + cos_theta * normal);
	Vector3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_sqrd())) * normal;
	return r_out_perp + r_out_parallel;
}
