#ifndef MATRIX4X4_HPP
# define MATRIX4X4_HPP

# include "Vector3.cuh"
# include "Point3.cuh"

namespace maths
{

	class Matrix4x4
	{
		private:
			float	m[4][4];
		public:
			RT_CALLABLE Matrix4x4();
			RT_CALLABLE Matrix4x4(const float values[4][4]);
			RT_CALLABLE Matrix4x4(const Matrix4x4 &other) = default;
			RT_CALLABLE ~Matrix4x4() = default;

			Matrix4x4	&operator=(const Matrix4x4 &other) = default;

			inline RT_CALLABLE float	&operator()(int row, int col) { return m[row][col];}
			inline RT_CALLABLE float	operator()(int row, int col) const { return m[row][col];}

			inline RT_CALLABLE Matrix4x4	inverse() const
			{
				Matrix4x4	inv;
				float		det;
				inv(0, 0) = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] - m[2][1] * m[1][2] * m[3][3] + m[2][1] * m[1][3] * m[3][2] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];
				inv(0, 1) = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] + m[2][1] * m[0][2] * m[3][3] - m[2][1] * m[0][3] * m[3][2] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];
				inv(0, 2) = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] - m[1][1] * m[0][2] * m[3][3] + m[1][1] * m[0][3] * m[3][2] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];
				inv(0, 3) = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] + m[1][1] * m[0][2] * m[2][3] - m[1][1] * m[0][3] * m[2][2] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];
				inv(1, 0) = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] + m[2][0] * m[1][2] * m[3][3] - m[2][0] * m[1][3] * m[3][2] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];
				inv(1, 1) = m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] - m[2][0] * m[0][2] * m[3][3] + m[2][0] * m[0][3] * m[3][2] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];
				inv(1, 2) = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] + m[1][0] * m[0][2] * m[3][3] - m[1][0] * m[0][3] * m[3][2] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];
				inv(1, 3) = m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] - m[1][0] * m[0][2] * m[2][3] + m[1][0] * m[0][3] * m[2][2] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];
				inv(2, 0) = m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] - m[2][0] * m[1][1] * m[3][3] + m[2][0] * m[1][3] * m[3][1] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];
				inv(2, 1) = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] + m[2][0] * m[0][1] * m[3][3] - m[2][0] * m[0][3] * m[3][1] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];
				inv(2, 2) = m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] - m[1][0] * m[0][1] * m[3][3] + m[1][0] * m[0][3] * m[3][1] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];
				inv(2, 3) = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] + m[1][0] * m[0][1] * m[2][3] - m[1][0] * m[0][3] * m[2][1] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];
				inv(3, 0) = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] + m[2][0] * m[1][1] * m[3][2] - m[2][0] * m[1][2] * m[3][1] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];
				inv(3, 1) = m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] - m[2][0] * m[0][1] * m[3][2] + m[2][0] * m[0][2] * m[3][1] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];
				inv(3, 2) = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] + m[1][0] * m[0][1] * m[3][2] - m[1][0] * m[0][2] * m[3][1] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];
				inv(3, 3) = m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] - m[1][0] * m[0][1] * m[2][2] + m[1][0] * m[0][2] * m[2][1] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];
				det = m[0][0] * inv(0, 0) + m[0][1] * inv(1, 0) + m[0][2] * inv(2, 0) + m[0][3] * inv(3, 0);
				if (det == 0.0f)
						return Matrix4x4();
				for (int i = 0;i < 4;++i)
						for (int j = 0;j < 4;++j)
								inv(i, j) /= det;
				return inv;
			}

			inline RT_CALLABLE Matrix4x4	transpose() const
			{
				Matrix4x4	transposed;
				for (int i = 0;i < 4;++i)
					for (int j = 0;j < 4;++j)
						transposed(i, j) = m[j][i];
				return transposed;
			}
	};

	std::ostream	&operator<<(std::ostream &os, const Matrix4x4 &mat);

	inline RT_CALLABLE Matrix4x4	operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
	{
		Matrix4x4	result;
		for (int row = 0;row < 4;++row)
		{
			for (int col = 0;col < 4;++col)
			{
				result(row, col) = mat1(row, 0) * mat2(0, col) +
								   mat1(row, 1) * mat2(1, col) +
								   mat1(row, 2) * mat2(2, col) +
								   mat1(row, 3) * mat2(3, col);
			}
		}
		return result;
	}

	inline RT_CALLABLE Vector3	operator*(const Matrix4x4 &mat, const Vector3 &vec)
	{
		float	x = mat(0, 0) * vec.x() + mat(0, 1) * vec.y() + mat(0, 2) * vec.z();
		float	y = mat(1, 0) * vec.x() + mat(1, 1) * vec.y() + mat(1, 2) * vec.z();
		float	z = mat(2, 0) * vec.x() + mat(2, 1) * vec.y() + mat(2, 2) * vec.z();
		return Vector3(x, y, z);
	}

	inline RT_CALLABLE Point3	operator*(const Matrix4x4 &mat, const Point3 &point)
	{
		float	x = mat(0, 0) * point.x() + mat(0, 1) * point.y() + mat(0, 2) * point.z() + mat(0, 3);
		float	y = mat(1, 0) * point.x() + mat(1, 1) * point.y() + mat(1, 2) * point.z() + mat(1, 3);
		float	z = mat(2, 0) * point.x() + mat(2, 1) * point.y() + mat(2, 2) * point.z() + mat(2, 3);
		return Point3(x, y, z);
	}

	RT_CALLABLE Matrix4x4	translationMatrix(const Vector3 &translation);
	RT_CALLABLE Matrix4x4	rotationMatrix(const Vector3 &rotation);
	RT_CALLABLE Matrix4x4	scaleMatrix(const Vector3 &scale);
	RT_CALLABLE Matrix4x4	masterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);
	RT_CALLABLE Matrix4x4	inverseTranslationMatrix(const Vector3 &translation);
	RT_CALLABLE Matrix4x4	inverseRotationMatrix(const Vector3 &rotation);
	RT_CALLABLE Matrix4x4	inverseScaleMatrix(const Vector3 &scale);
	RT_CALLABLE Matrix4x4	inverseMasterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);

} // namespace maths

#endif /* MATRIX4X4_HPP */
