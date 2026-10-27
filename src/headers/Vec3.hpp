#ifndef VEC3_HPP
# define VEC3_HPP

# ifndef EPSILON
#  define EPSILON 1e-8f
# endif // EPSILON

struct Vec3 {
	float x = 0.0f, y = 0.0f, z = 0.0f;

	constexpr Vec3() = default;
	constexpr Vec3(float x_, float y_, float z_);

	constexpr Vec3	operator-() const;
	constexpr float	operator[](int i) const;
	inline float&	operator[](int i);

	constexpr Vec3&	operator+=(const Vec3& other);
	constexpr Vec3&	operator-=(const Vec3& other);
	constexpr Vec3&	operator*=(const Vec3& other);
	constexpr Vec3&	operator*=(float t);
	constexpr Vec3&	operator/=(float t);

	inline float	lengthSquared() const;
	inline float	length() const;
	inline bool		nearZero();
	inline Vec3		normalize();
};

using Color = Vec3;
using Point3 = Vec3;

constexpr Vec3	operator+(Vec3 v, const Vec3& w);
constexpr Vec3	operator-(Vec3 v, const Vec3& w);
constexpr Vec3	operator*(Vec3 v, const Vec3& w);
constexpr Vec3	operator*(Vec3 v, float t);
constexpr Vec3	operator*(float t, Vec3 v);
constexpr Vec3	operator/(Vec3 v, float t);

constexpr float	dot(const Vec3& v, const Vec3& w);
constexpr Vec3	cross(const Vec3& v, const Vec3& w);
constexpr Vec3	lerp(const Vec3& v, const Vec3& w, float t);
constexpr Vec3	reflect(const Vec3& v, const Vec3& n);
inline Vec3		refract(const Vec3& uv, const Vec3& n, float etaiOverEtat);

# include "detail/Vec3.ipp"

#endif // VEC3_HPP
