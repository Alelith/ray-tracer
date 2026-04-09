#ifndef VECTOR3_HPP
# define VECTOR3_HPP

# include <cmath>
# include <ostream>

# ifdef __CUDACC__
#  define RT_CALLABLE __host__ __device__
# else
#  define RT_CALLABLE
# endif

# ifdef __CUDACC__
#  define RT_KERNEL __global__
# else
#  define RT_KERNEL
# endif

class Vector3
{
	private:
		float	v[3];
	public:
		RT_CALLABLE Vector3();
		RT_CALLABLE Vector3(float x, float y, float z);
		RT_CALLABLE Vector3(const Vector3 &other) = default;
		RT_CALLABLE ~Vector3() = default;

		RT_CALLABLE Vector3			&operator=(const Vector3 &other) = default;

		inline RT_CALLABLE Vector3	operator-() const { return Vector3(-v[0], -v[1], -v[2]); }
		inline RT_CALLABLE float	&operator[](int index) { return v[index]; }
		inline RT_CALLABLE float	operator[](int index) const { return v[index]; }

		inline RT_CALLABLE Vector3 	operator+=(const Vector3 &other) { v[0] += other[0]; v[1] += other[1]; v[2] += other[2]; return *this; }
		inline RT_CALLABLE Vector3 	operator-=(const Vector3 &other) { v[0] -= other[0]; v[1] -= other[1]; v[2] -= other[2]; return *this; }
		inline RT_CALLABLE Vector3 	operator*=(const Vector3 &other) { v[0] *= other[0]; v[1] *= other[1]; v[2] *= other[2]; return *this; }
		inline RT_CALLABLE Vector3 	operator*=(float scalar) { v[0] *= scalar; v[1] *= scalar; v[2] *= scalar; return *this; }
		inline RT_CALLABLE Vector3 	operator/=(float scalar) { v[0] /= scalar; v[1] /= scalar; v[2] /= scalar; return *this; }

		inline RT_CALLABLE float	lengthSqrd() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }
		inline RT_CALLABLE float	length() const { return sqrtf(lengthSqrd()); }
		inline RT_CALLABLE Vector3	normalized() const { float len = length(); return (len != 0.0f) ? Vector3(v[0] / len, v[1] / len, v[2] / len) : Vector3(0, 0, 0); }

		inline RT_CALLABLE bool		isNearZero() const { return lengthSqrd() < 1e-6; }

		inline RT_CALLABLE Vector3	clamp(float minVal, float maxVal) const
		{
			return Vector3(fmaxf(minVal, fminf(maxVal, v[0])),
						 fmaxf(minVal, fminf(maxVal, v[1])),
						 fmaxf(minVal, fminf(maxVal, v[2])));
		}

		inline RT_CALLABLE float	x() const { return v[0]; }
		inline RT_CALLABLE float	y() const { return v[1]; }
		inline RT_CALLABLE float	z() const { return v[2]; }
};

using Color = Vector3;

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec);

inline RT_CALLABLE Vector3	operator+(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() + vec2.x(), vec1.y() + vec2.y(), vec1.z() + vec2.z()); }
inline RT_CALLABLE Vector3	operator-(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() - vec2.x(), vec1.y() - vec2.y(), vec1.z() - vec2.z()); }
inline RT_CALLABLE Vector3	operator*(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() * vec2.x(), vec1.y() * vec2.y(), vec1.z() * vec2.z()); }
inline RT_CALLABLE Vector3	operator*(float scalar, const Vector3 &vec) { return Vector3(scalar * vec.x(), scalar * vec.y(), scalar * vec.z()); }
inline RT_CALLABLE Vector3	operator*(const Vector3 &vec, float scalar) { return Vector3(scalar * vec.x(), scalar * vec.y(), scalar * vec.z()); }
inline RT_CALLABLE Vector3	operator/(const Vector3 &vec, float scalar) { return Vector3(vec.x() / scalar, vec.y() / scalar, vec.z() / scalar); }

inline RT_CALLABLE float	dot(const Vector3 &vec1, const Vector3 &vec2) { return vec1.x() * vec2.x() + vec1.y() * vec2.y() + vec1.z() * vec2.z(); }
inline RT_CALLABLE Vector3	cross(const Vector3 &vec1, const Vector3 &vec2)
{
	return Vector3(vec1.y() * vec2.z() - vec1.z() * vec2.y(),
					vec1.z() * vec2.x() - vec1.x() * vec2.z(),
					vec1.x() * vec2.y() - vec1.y() * vec2.x());
}

inline RT_CALLABLE Vector3	reflect(const Vector3 &vec, const Vector3 &normal) { return vec - 2 * dot(vec, normal) * normal; }

inline RT_CALLABLE Vector3	refract(const Vector3 &uv, const Vector3 &normal, float eta_over_eta_prime)
{
	float cos_theta = fminf(dot(-uv, normal), 1.0f);
	Vector3 r_out_perp = eta_over_eta_prime * (uv + cos_theta * normal);
	Vector3 r_out_parallel = -sqrtf(fabsf(1.0f - r_out_perp.lengthSqrd())) * normal;
	return r_out_perp + r_out_parallel;
}

inline RT_CALLABLE float	reflectance(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

#endif /* VECTOR3_HPP */
