#ifndef VECTOR3_HPP
# define VECTOR3_HPP

# include <cmath>
# include <ostream>

class Vector3
{
	private:
		double	v[4];
	public:
		Vector3();
		Vector3(double x, double y, double z, double w = 0.0);
		Vector3(const Vector3 &other);
		~Vector3();

		static const	Vector3 Zero;
		static const	Vector3 Up;
		static const	Vector3 Right;
		static const	Vector3 Forward;
		static const	Vector3 Down;
		static const	Vector3 Left;
		static const	Vector3 Backward;
		static const	Vector3 One;

		Vector3			&operator=(const Vector3 &other);

		Vector3			operator-() const;
		double			&operator[](int index);
		double			operator[](int index) const;

		Vector3 		operator+=(const Vector3 &other);
		Vector3 		operator-=(const Vector3 &other);
		Vector3 		operator*=(const Vector3 &other);
		Vector3 		operator*=(double scalar);
		Vector3 		operator/=(double scalar);

		double			length_sqrd() const;
		double			length() const;
		Vector3			normalized() const;

		bool			isNearZero() const;
};

using Point3 = Vector3;
using Color = Vector3;

std::ostream	&operator<<(std::ostream &os, const Vector3 &vec);

Vector3			operator+(const Vector3 &vec1, const Vector3 &vec2);
Vector3			operator-(const Vector3 &vec1, const Vector3 &vec2);

Vector3			operator*(const Vector3 &vec1, const Vector3 &vec2);
Vector3			operator*(double scalar, const Vector3 &vec);
Vector3			operator*(const Vector3 &vec, double scalar);
Vector3			operator/(const Vector3 &vec, double scalar);

double			dot(const Vector3 &vec1, const Vector3 &vec2);
Vector3			cross(const Vector3 &vec1, const Vector3 &vec2);

Vector3			reflect(const Vector3 &vec, const Vector3 &normal);
Vector3			refract(const Vector3 &uv, const Vector3 &normal, double eta_over_eta_prime);

#endif /* VECTOR3_HPP */
