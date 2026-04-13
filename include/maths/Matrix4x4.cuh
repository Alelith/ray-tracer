/**
 * @file Matrix4x4.cuh
 * @brief 4x4 Matrix class and related affine transformations
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef MATRIX4X4_HPP
# define MATRIX4X4_HPP

# include "Vector3.cuh"
# include "Point3.cuh"

/**
 * @class Matrix4x4
 * @brief 4x4 matrix structure for geometric transformations
 * 
 * Affine transformations representation. Used for object placement
 * and camera manipulation within the scene. Contains basic matrix methods
 * like transposition and inversion.
 * 
 * @ingroup math_module
 */
class Matrix4x4
{
	private:
		float	m[4][4]; ///< Internal 2D array representation of the matrix
	public:
		RT_CALLABLE Matrix4x4();
		RT_CALLABLE Matrix4x4(const float values[4][4]);
		RT_CALLABLE Matrix4x4(const Matrix4x4 &other) = default;
		RT_CALLABLE ~Matrix4x4() = default;

		Matrix4x4						&operator=(const Matrix4x4 &other) = default;

		inline RT_CALLABLE float		&operator()(int row, int col) { return m[row][col]; }
		inline RT_CALLABLE float		operator()(int row, int col) const { return m[row][col]; }

		/**
		 * @brief Computes the algebraic inverse of the matrix
		 * 
		 * @return The inverse matrix or a default matrix if singular
		 */
		inline RT_CALLABLE Matrix4x4	inverse() const
		{
			Matrix4x4 inv;
			float det;

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
					return Matrix4x4(); // Retorna una matriz nula o identidad según tu preferencia
			for (int i = 0; i < 4; ++i)
					for (int j = 0; j < 4; ++j)
							inv(i, j) /= det;
			return inv;
		}

		/**
		 * @brief Computes transposed matrix
		 * 
		 * @return Transposed result
		 */
		inline RT_CALLABLE Matrix4x4	transpose() const
		{
			Matrix4x4 transposed;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					transposed(i, j) = m[j][i];
			return transposed;
		}
};

std::ostream	&operator<<(std::ostream &os, const Matrix4x4 &mat);

/**
 * @brief Multiplies two 4x4 matrices
 * 
 * @ingroup math_module
 * 
 * @param[in] mat1 Left matrix operand
 * @param[in] mat2 Right matrix operand
 * @return Concatenated matrix transformation
 */
inline RT_CALLABLE Matrix4x4	operator*(const Matrix4x4 &mat1, const Matrix4x4 &mat2)
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

/**
 * @brief Multiplies a vector by a matrix (no translation)
 * 
 * @ingroup math_module
 * 
 * @param[in] mat The matrix
 * @param[in] vec The vector to evaluate
 * @return Computed vector mapped
 */
inline RT_CALLABLE Vector3	operator*(const Matrix4x4 &mat, const Vector3 &vec)
{
	float x = mat(0, 0) * vec.x() + mat(0, 1) * vec.y() + mat(0, 2) * vec.z();
	float y = mat(1, 0) * vec.x() + mat(1, 1) * vec.y() + mat(1, 2) * vec.z();
	float z = mat(2, 0) * vec.x() + mat(2, 1) * vec.y() + mat(2, 2) * vec.z();
	return Vector3(x, y, z);
}

/**
 * @brief Multiplies a point by a matrix (applies translation)
 * 
 * @ingroup math_module
 * 
 * @param[in] mat The matrix
 * @param[in] point The point
 * @return Point after transformation mapping
 */
inline RT_CALLABLE Point3	operator*(const Matrix4x4 &mat, const Point3 &point)
{
	float x = mat(0, 0) * point.x() + mat(0, 1) * point.y() + mat(0, 2) * point.z() + mat(0, 3);
	float y = mat(1, 0) * point.x() + mat(1, 1) * point.y() + mat(1, 2) * point.z() + mat(1, 3);
	float z = mat(2, 0) * point.x() + mat(2, 1) * point.y() + mat(2, 2) * point.z() + mat(2, 3);
	return Point3(x, y, z);
}

/**
 * @brief Constructs a new generic translation matrix
 * 
 * @ingroup math_module
 * 
 * @param[in] translation XYZ amounts
 * @return 4x4 matrix
 */
RT_CALLABLE Matrix4x4		translationMatrix(const Vector3 &translation);

/**
 * @brief Constructs a generic rotation matrix
 * 
 * @ingroup math_module
 * 
 * @param[in] rotation Pitch, Yaw and Roll mapped to X, Y, Z degrees
 * @return 4x4 matrix
 */
RT_CALLABLE Matrix4x4		rotationMatrix(const Vector3 &rotation);

/**
 * @brief Constructs a new geometric scale matrix
 * 
 * @ingroup math_module
 * 
 * @param[in] scale Scaling amounts for X, Y, Z dimensions
 * @return 4x4 matrix
 */
RT_CALLABLE Matrix4x4		scaleMatrix(const Vector3 &scale);

/**
 * @brief Generates consolidated 4x4 matrix from geometric parameters
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Offset vector
 * @param[in] rotation Angles parameterization
 * @param[in] scale Dimension extents mapping
 * @return Merged 4x4 matrix
 */
RT_CALLABLE Matrix4x4		masterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);

/**
 * @brief Creates the inverse of a given translation vector as matrix
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Initial offsets mapping
 * @return Inversed equivalent 4x4 matrix
 */
RT_CALLABLE Matrix4x4		inverseTranslationMatrix(const Vector3 &translation);

/**
 * @brief Generates inverse evaluation of rotation values
 * 
 * @ingroup math_module
 * 
 * @param[in] rotation Amounts mapped to degrees natively equivalent mapped
 * @return Reverse rotation 4x4 matrix
 */
RT_CALLABLE Matrix4x4		inverseRotationMatrix(const Vector3 &rotation);

/**
 * @brief Precalculates specific inversal structure for scale
 * 
 * @ingroup math_module
 * 
 * @param[in] scale Base dimension equivalents
 * @return Scaling mapping into an inverted 4x4 matrix
 */
RT_CALLABLE Matrix4x4		inverseScaleMatrix(const Vector3 &scale);

/**
 * @brief Produces a full reverse matrix out of separated transformations
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Normal mapping for space mappings
 * @param[in] rotation The mapped equivalents degrees parameter
 * @param[in] scale Scaling amounts mapping bounds
 * @return Consolidated inverse Master rotation Matrix
 */
RT_CALLABLE Matrix4x4		inverseMasterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);

#endif /* MATRIX4X4_HPP */
