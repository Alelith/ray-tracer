#ifndef VECTOR3_HPP
# define VECTOR3_HPP

# include <cmath>
# include <ostream>

class Vector3
{
	private:
		float	v[3];
	public:
		Vector3();
		Vector3(float x, float y, float z);
		Vector3(const Vector3 &other) = default;
		~Vector3() = default;

		static const	Vector3 Zero;
		static const	Vector3 Up;
		static const	Vector3 Right;
		static const	Vector3 Forward;
		static const	Vector3 Down;
		static const	Vector3 Left;
		static const	Vector3 Backward;
		static const	Vector3 One;

		static const	Vector3 White;
		static const	Vector3 Black;
		static const	Vector3 Gray;
		static const	Vector3 LightGray;
		static const	Vector3 DarkGray;
		static const	Vector3 Red;
		static const	Vector3 Green;
		static const	Vector3 Blue;
		static const	Vector3 Yellow;
		static const	Vector3 Cyan;
		static const	Vector3 Magenta;

		Vector3			&operator=(const Vector3 &other) = default;

		inline Vector3		operator-() const { return Vector3(-v[0], -v[1], -v[2]); }
		inline float		&operator[](int index) { return v[index]; }
		inline float		operator[](int index) const { return v[index]; }

		inline Vector3 		operator+=(const Vector3 &other) { v[0] += other[0]; v[1] += other[1]; v[2] += other[2]; return *this; }
		inline Vector3 		operator-=(const Vector3 &other) { v[0] -= other[0]; v[1] -= other[1]; v[2] -= other[2]; return *this; }
		inline Vector3 		operator*=(const Vector3 &other) { v[0] *= other[0]; v[1] *= other[1]; v[2] *= other[2]; return *this; }
		inline Vector3 		operator*=(float scalar) { v[0] *= scalar; v[1] *= scalar; v[2] *= scalar; return *this; }
		inline Vector3 		operator/=(float scalar) { v[0] /= scalar; v[1] /= scalar; v[2] /= scalar; return *this; }

		inline float		length_sqrd() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }
		inline float		length() const { return std::sqrt(length_sqrd()); }
		inline Vector3		normalized() const { float len = length(); return (len != 0.0f) ? Vector3(v[0] / len, v[1] / len, v[2] / len) : Vector3::Zero; }

		inline bool			isNearZero() const { return length_sqrd() < 1e-6; }

		inline float		x() const { return v[0]; }
		inline float		y() const { return v[1]; }
		inline float		z() const { return v[2]; }
};

using Color = Vector3;

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec);

inline Vector3			operator+(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() + vec2.x(), vec1.y() + vec2.y(), vec1.z() + vec2.z()); }
inline Vector3			operator-(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() - vec2.x(), vec1.y() - vec2.y(), vec1.z() - vec2.z()); }
inline Vector3			operator*(const Vector3 &vec1, const Vector3 &vec2) { return Vector3(vec1.x() * vec2.x(), vec1.y() * vec2.y(), vec1.z() * vec2.z()); }
inline Vector3			operator*(float scalar, const Vector3 &vec) { return Vector3(scalar * vec.x(), scalar * vec.y(), scalar * vec.z()); }
inline Vector3			operator*(const Vector3 &vec, float scalar) { return Vector3(scalar * vec.x(), scalar * vec.y(), scalar * vec.z()); }
inline Vector3			operator/(const Vector3 &vec, float scalar) { return Vector3(vec.x() / scalar, vec.y() / scalar, vec.z() / scalar); }

inline float			dot(const Vector3 &vec1, const Vector3 &vec2) { return vec1.x() * vec2.x() + vec1.y() * vec2.y() + vec1.z() * vec2.z(); }
inline Vector3			cross(const Vector3 &vec1, const Vector3 &vec2)
{
	return Vector3(vec1.y() * vec2.z() - vec1.z() * vec2.y(),
					vec1.z() * vec2.x() - vec1.x() * vec2.z(),
					vec1.x() * vec2.y() - vec1.y() * vec2.x());
}

inline Vector3	reflect(const Vector3 &vec, const Vector3 &normal) { return vec - 2 * dot(vec, normal) * normal; }

inline Vector3	refract(const Vector3 &uv, const Vector3 &normal, float eta_over_eta_prime)
{
	float cos_theta = std::fmin(dot(-uv, normal), 1.0);
	Vector3 r_out_perp = eta_over_eta_prime * (uv + cos_theta * normal);
	Vector3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_sqrd())) * normal;
	return r_out_perp + r_out_parallel;
}

inline float	reflectance(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

#endif /* VECTOR3_HPP */
