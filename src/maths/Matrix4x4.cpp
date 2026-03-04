#include "Matrix4x4.hpp"

Matrix4x4::Matrix4x4() : m{{0.0, 0.0, 0.0, 0.0},
							 {0.0, 0.0, 0.0, 0.0},
							 {0.0, 0.0, 0.0, 0.0},
							 {0.0, 0.0, 0.0, 1.0}} {}

Matrix4x4::Matrix4x4(const double values[4][4])
{	
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m[i][j] = values[i][j];
}


static const double	s_identity[4][4] = {{1.0, 0.0, 0.0, 0.0},
										{0.0, 1.0, 0.0, 0.0},
										{0.0, 0.0, 1.0, 0.0},
										{0.0, 0.0, 0.0, 1.0}};
const Matrix4x4	Matrix4x4::Identity(s_identity);

double	&Matrix4x4::operator()(int row, int col)
{ return m[row][col]; }

double	Matrix4x4::operator()(int row, int col) const
{ return m[row][col]; }

std::ostream	&operator<<(std::ostream &os, const Matrix4x4 &mat)
{
	os << "Matrix4x4(\n";
	for (int i = 0; i < 4; ++i)
	{
		os << "  [";
		for (int j = 0; j < 4; ++j)
		{
			os << mat(i, j);
			if (j < 3)
				os << ", ";
		}
		os << "]";
		if (i < 3)
			os << ",\n";
	}
	os << "\n)";
	return os;
}

Matrix4x4	operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			result(i, j) = 0.0;
			for (int k = 0; k < 4; ++k)
				result(i, j) += mat1(i, k) * mat2(k, j);
		}
	return result;
}

Vector3	operator*(const Matrix4x4 &mat, const Vector3 &vec)
{
	double x = mat(0, 0) * vec[0] + mat(0, 1) * vec[1] + mat(0, 2) * vec[2] + mat(0, 3) * vec[3];
	double y = mat(1, 0) * vec[0] + mat(1, 1) * vec[1] + mat(1, 2) * vec[2] + mat(1, 3) * vec[3];
	double z = mat(2, 0) * vec[0] + mat(2, 1) * vec[1] + mat(2, 2) * vec[2] + mat(2, 3) * vec[3];
	double w = mat(3, 0) * vec[0] + mat(3, 1) * vec[1] + mat(3, 2) * vec[2] + mat(3, 3) * vec[3];
	if (w != 0.0)
	{
		x /= w;
		y /= w;
		z /= w;
	}
	return Vector3(x, y, z);
}

Matrix4x4	translationMatrix(const Vector3 &translation)
{
	Matrix4x4 result = Matrix4x4::Identity;
	result(0, 3) = translation[0];
	result(1, 3) = translation[1];
	result(2, 3) = translation[2];
	return result;
}

Matrix4x4	rotationMatrix(const Vector3 &rotation)
{
	Matrix4x4 result = Matrix4x4::Identity;
	Vector3 newY = rotation.normalized();
	Vector3 newX = Vector3(0.0, 1.0, 0.0).normalized();
	if (std::abs(newY[0]) < 0.999)
		newX = cross(newY, Vector3(1.0, 0.0, 0.0)).normalized();
	else
		newX = cross(newY, Vector3(0.0, 0.0, 1.0)).normalized();
	Vector3 newZ = cross(newX, newY).normalized();
	result(0, 0) = newX[0];
	result(0, 1) = newX[1];
	result(0, 2) = newX[2];
	result(1, 0) = newY[0];
	result(1, 1) = newY[1];
	result(1, 2) = newY[2];
	result(2, 0) = newZ[0];
	result(2, 1) = newZ[1];
	result(2, 2) = newZ[2];
	return result;
}

Matrix4x4	scaleMatrix(const Vector3 &scale)
{
	Matrix4x4 result = Matrix4x4::Identity;
	result(0, 0) = scale[0];
	result(1, 1) = scale[1];
	result(2, 2) = scale[2];
	return result;
}

Matrix4x4	inverseTranslationMatrix(const Vector3 &translation)
{
	Matrix4x4 result = Matrix4x4::Identity;
	result(0, 3) = -translation[0];
	result(1, 3) = -translation[1];
	result(2, 3) = -translation[2];
	return result;
}

Matrix4x4	inverseRotationMatrix(const Vector3 &rotation)
{
	Matrix4x4 rotMat = rotationMatrix(rotation);
	return transpose(rotMat);
}

Matrix4x4	inverseScaleMatrix(const Vector3 &scale)
{
	Matrix4x4 result = Matrix4x4::Identity;
	result(0, 0) = (scale[0] != 0.0) ? 1.0 / scale[0] : 0.0;
	result(1, 1) = (scale[1] != 0.0) ? 1.0 / scale[1] : 0.0;
	result(2, 2) = (scale[2] != 0.0) ? 1.0 / scale[2] : 0.0;
	return result;
}

Matrix4x4	transpose(const Matrix4x4 &mat)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result(i, j) = mat(j, i);
	return result;
}
