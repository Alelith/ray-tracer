#ifndef MATRIX4X4_HPP
# define MATRIX4X4_HPP

# include "Vector3.hpp"
# include "Point3.hpp"

class Matrix4x4
{
	private:
		float	m[4][4];
	public:
		Matrix4x4();
		Matrix4x4(const float values[4][4]);
		Matrix4x4(const Matrix4x4 &other) = default;
		~Matrix4x4() = default;

		static const	Matrix4x4 Identity;

		Matrix4x4		&operator=(const Matrix4x4 &other) = default;

		float			&operator()(int row, int col);
		float			operator()(int row, int col) const;
};

std::ostream	&operator<<(std::ostream &os, const Matrix4x4 &mat);

Matrix4x4		operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2);

// Matrix * Vector: w=0, no translation applied (directions)
Vector3			operator*(const Matrix4x4 &mat, const Vector3 &vec);

// Matrix * Point: w=1, translation applied (positions)
Point3			operator*(const Matrix4x4 &mat, const Point3 &point);

Matrix4x4		translationMatrix(const Vector3 &translation);
Matrix4x4		rotationMatrix(const Vector3 &rotation);
Matrix4x4		scaleMatrix(const Vector3 &scale);

Matrix4x4		inverseTranslationMatrix(const Vector3 &translation);
Matrix4x4		inverseRotationMatrix(const Vector3 &rotation);
Matrix4x4		inverseScaleMatrix(const Vector3 &scale);

Matrix4x4		transpose(const Matrix4x4 &mat);

#endif /* MATRIX4X4_HPP */
