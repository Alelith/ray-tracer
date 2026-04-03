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

		inline float			&operator()(int row, int col) { return m[row][col]; }
		inline float			operator()(int row, int col) const { return m[row][col]; }
};

std::ostream	&operator<<(std::ostream &os, const Matrix4x4 &mat);

inline Matrix4x4		operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
{
	Matrix4x4 result;
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			result(row, col) = mat1(row, 0) * mat2(0, col) +
							   mat1(row, 1) * mat2(1, col) +
							   mat1(row, 2) * mat2(2, col) +
							   mat1(row, 3) * mat2(3, col);
		}
	}
	return result;
}

// Matrix * Vector: w=0, no translation applied (directions)
inline Vector3			operator*(const Matrix4x4 &mat, const Vector3 &vec)
{
	float x = mat(0, 0) * vec.x() + mat(0, 1) * vec.y() + mat(0, 2) * vec.z();
	float y = mat(1, 0) * vec.x() + mat(1, 1) * vec.y() + mat(1, 2) * vec.z();
	float z = mat(2, 0) * vec.x() + mat(2, 1) * vec.y() + mat(2, 2) * vec.z();
	return Vector3(x, y, z);
}

// Matrix * Point: w=1, translation applied (positions)
inline Point3			operator*(const Matrix4x4 &mat, const Point3 &point)
{
	float x = mat(0, 0) * point.x() + mat(0, 1) * point.y() + mat(0, 2) * point.z() + mat(0, 3);
	float y = mat(1, 0) * point.x() + mat(1, 1) * point.y() + mat(1, 2) * point.z() + mat(1, 3);
	float z = mat(2, 0) * point.x() + mat(2, 1) * point.y() + mat(2, 2) * point.z() + mat(2, 3);
	return Point3(x, y, z);
}

Matrix4x4		translationMatrix(const Vector3 &translation);
Matrix4x4		rotationMatrix(const Vector3 &rotation);
Matrix4x4		scaleMatrix(const Vector3 &scale);

Matrix4x4		inverseTranslationMatrix(const Vector3 &translation);
Matrix4x4		inverseRotationMatrix(const Vector3 &rotation);
Matrix4x4		inverseScaleMatrix(const Vector3 &scale);

Matrix4x4		transpose(const Matrix4x4 &mat);

#endif /* MATRIX4X4_HPP */
