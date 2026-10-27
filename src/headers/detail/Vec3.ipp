#include <cmath>

constexpr Vec3::Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

constexpr Vec3 Vec3::operator-() const { return {-x, -y, -z}; }
constexpr float Vec3::operator[](int i) const { return i == 0 ? x : i == 1 ? y : z; }
inline float& Vec3::operator[](int i) { return i == 0 ? x : i == 1 ? y : z; }

constexpr Vec3& Vec3::operator+=(const Vec3& other) {
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}
constexpr Vec3& Vec3::operator-=(const Vec3& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}
constexpr Vec3& Vec3::operator*=(const Vec3& other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}
constexpr Vec3& Vec3::operator*=(float t) {
	x *= t;
	y *= t;
	z *= t;
}
constexpr Vec3& Vec3::operator/=(float t) { return *this *= (1.0f / t); }

inline float Vec3::lengthSquared() const { return x*x + y*y + z*z; }
inline float Vec3::length() const {return std::sqrtf(lengthSquared()); }
inline bool Vec3::nearZero() { return std::fabsf(x) < EPSILON && std::fabsf(y) < EPSILON && std::fabsf(z) < EPSILON; }
inline Vec3 Vec3::normalize() { return *this / length(); }

using Color = Vec3;
using Point3 = Vec3;

constexpr Vec3 operator+(Vec3 v, const Vec3& w) { return v += w; }
constexpr Vec3	operator-(Vec3 v, const Vec3& w) { return v -= w; }
constexpr Vec3	operator*(Vec3 v, const Vec3& w) { return v *= w; }
constexpr Vec3	operator*(Vec3 v, float t) { return v *= t; }
constexpr Vec3	operator*(float t, Vec3 v) { return v *= t; }
constexpr Vec3	operator/(Vec3 v, float t) { return v /= t; }

constexpr float	dot(const Vec3& v, const Vec3& w) { return v.x*w.x + v.y*w.y + v.z*w.z; }
constexpr Vec3	cross(const Vec3& v, const Vec3& w) { return {v.y*w.z - v.z*w.y, v.z*w.x - v.x*w.z, v.x*w.y - v.y*w.x}; }
constexpr Vec3	lerp(const Vec3& v, const Vec3& w, float t) { return (1.0f - t) * v + t * w; }
constexpr Vec3	reflect(const Vec3& v, const Vec3& n) { return v - 2.0f * dot(v, n) * n; }
constexpr Vec3	refract(const Vec3& uv, const Vec3& n, float etaiOverEtat) {
	float cosTheta = std::fminf(dot(-uv, n), 1.0f);

	Vec3 rPerp = etaiOverEtat * (uv + cosTheta * n);
	Vec3 rParall = -std::sqrtf(std::fabsf(1.0f - rPerp.lengthSquared())) * n;

	return rPerp + rParall;
}
