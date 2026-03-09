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

		Vector3			&operator=(const Vector3 &other) = default;

		Vector3			operator-() const;
		float			&operator[](int index);
		float			operator[](int index) const;

		Vector3 		operator+=(const Vector3 &other);
		Vector3 		operator-=(const Vector3 &other);
		Vector3 		operator*=(const Vector3 &other);
		Vector3 		operator*=(float scalar);
		Vector3 		operator/=(float scalar);

		float			length_sqrd() const;
		float			length() const;
		Vector3			normalized() const;

		bool			isNearZero() const;

		float			x() const;
		float			y() const;
		float			z() const;
};

using Color = Vector3;

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec);

Vector3			operator+(const Vector3 &vec1, const Vector3 &vec2);
Vector3			operator-(const Vector3 &vec1, const Vector3 &vec2);

Vector3			operator*(const Vector3 &vec1, const Vector3 &vec2);
Vector3			operator*(float scalar, const Vector3 &vec);
Vector3			operator*(const Vector3 &vec, float scalar);
Vector3			operator/(const Vector3 &vec, float scalar);

float			dot(const Vector3 &vec1, const Vector3 &vec2);
Vector3			cross(const Vector3 &vec1, const Vector3 &vec2);

inline Vector3	reflect(const Vector3 &vec, const Vector3 &normal)
{
	return vec - 2 * dot(vec, normal) * normal;
}

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
