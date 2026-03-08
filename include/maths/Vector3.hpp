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

Vector3			reflect(const Vector3 &vec, const Vector3 &normal);
Vector3			refract(const Vector3 &uv, const Vector3 &normal, float eta_over_eta_prime);

#endif /* VECTOR3_HPP */
