/**
 * @file Matrix4x4.cu
 * @brief Implementation of the 4x4 coordinate Matrix structure and transformations
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#include "Matrix4x4.cuh"
#include "Point3.cuh"

/**
 * @brief Default constructor initializing to an identity matrix
 * 
 * @ingroup math_module
 */
RT_CALLABLE Matrix4x4::Matrix4x4() : m{{0.0, 0.0, 0.0, 0.0},
							 {0.0, 0.0, 0.0, 0.0},
							 {0.0, 0.0, 0.0, 0.0},
							 {0.0, 0.0, 0.0, 1.0}} {}

/**
 * @brief Constructs a matrix from a 2D array
 * 
 * @ingroup math_module
 * 
 * @param[in] values Multidimensional array representation
 */
RT_CALLABLE Matrix4x4::Matrix4x4(const float values[4][4])
{	
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m[i][j] = values[i][j];
}

/*static const float	s_identity[4][4] = {{1.0, 0.0, 0.0, 0.0},
										{0.0, 1.0, 0.0, 0.0},
										{0.0, 0.0, 1.0, 0.0},
										{0.0, 0.0, 0.0, 1.0}};
const Matrix4x4	Matrix4x4::Identity(s_identity);*/

/**
 * @brief Standard output stream override for debugging
 * 
 * @ingroup math_module
 * 
 * @param[in,out] os Standard output stream
 * @param[in] mat Extracted matrix mapping structure
 * @return stream updated
 */
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

/**
 * @brief Generates mathematical standard translation transformation representation
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Displacements per coordinate axis mappings
 * @return Resultant generic bounds scaling matrix
 */
RT_CALLABLE Matrix4x4	translationMatrix(const Vector3 &translation)
{
	Matrix4x4 result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	result(0, 3) = translation[0];
	result(1, 3) = translation[1];
	result(2, 3) = translation[2];
	return result;
}

/**
 * @brief Produces a rotation representation combined of standard 3 axis Euler mappings
 * 
 * @ingroup math_module
 * 
 * @param[in] rotation Geometric pitch, yaw, and relative rolls mapped equivalently in degrees
 * @return Consolidated angular orientation matrix
 */
RT_CALLABLE Matrix4x4	rotationMatrix(const Vector3 &rotation)
{
	float radX = rotation[0] * M_PI_F / 180.0f;
	float radY = rotation[1] * M_PI_F / 180.0f;
	float radZ = rotation[2] * M_PI_F / 180.0f;

	float cx = cosf(radX), sx = sinf(radX);
	float cy = cosf(radY), sy = sinf(radY);
	float cz = cosf(radZ), sz = sinf(radZ);

	Matrix4x4 rx = Matrix4x4((const float[4][4]){
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, cx, -sx, 0.0f},
		{0.0f, sx, cx, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	});

	Matrix4x4 ry = Matrix4x4((const float[4][4]){
		{cy, 0.0f, sy, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{-sy, 0.0f, cy, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	});

	Matrix4x4 rz = Matrix4x4((const float[4][4]){
		{cz, -sz, 0.0f, 0.0f},
		{sz, cz, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	});

	return rz * ry * rx;
}

/**
 * @brief Expands factors on the coordinate plane axes diagonally
 * 
 * @ingroup math_module
 * 
 * @param[in] scale Values corresponding dimensions multiplier scale parameters
 * @return Transposed equivalents structural representation scale amounts as matrix bounds
 */
RT_CALLABLE Matrix4x4	scaleMatrix(const Vector3 &scale)
{
	Matrix4x4 result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	result(0, 0) = scale[0];
	result(1, 1) = scale[1];
	result(2, 2) = scale[2];
	return result;
}

/**
 * @brief Constructs a combined universal conversion structure mapping offsets dimensions parameters rotations simultaneously
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Extents translation offset distances mapped
 * @param[in] rotation Standard amounts angles Euler geometric
 * @param[in] scale Bounding extensions scaling constraints mapped axes distances
 * @return Full combined multiplied matrix representing spatial state modifications mapped out completely
 */
RT_CALLABLE Matrix4x4	masterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale)
{
	Matrix4x4 transMat = translationMatrix(translation);
	Matrix4x4 rotMat = rotationMatrix(rotation);
	Matrix4x4 scaleMat = scaleMatrix(scale);
	return transMat * rotMat * scaleMat;
}

/**
 * @brief Builds the inversion matrix parameters mapped distances translated inversed equivalents mapped back
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Standard input offsets parameters mapped translation coordinates
 * @return Mathematical mapped standard inversion generic matrix corresponding back offsets mappings coordinate dimensions bounds representations
 */
RT_CALLABLE Matrix4x4	inverseTranslationMatrix(const Vector3 &translation)
{
	Matrix4x4 result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	result(0, 3) = -translation[0];
	result(1, 3) = -translation[1];
	result(2, 3) = -translation[2];
	return result;
}

/**
 * @brief Constructs the inverse rotations equivalences dimensions properties geometric matrix parameters angles degrees properties structures transformations bounds mappings representations mathematical
 * 
 * @ingroup math_module
 * 
 * @param[in] rotation Pitch mapping dimensions axis rotation representations angles sizes properties lengths bounds extents spatial boundaries lengths coordinate axes Euler standard transformations angular
 * @return Inversed combined mathematical structure mapped representations bounding mapped sizes
 */
RT_CALLABLE Matrix4x4	inverseRotationMatrix(const Vector3 &rotation)
{
	float radX = rotation[0] * M_PI_F / 180.0f;
	float radY = rotation[1] * M_PI_F / 180.0f;
	float radZ = rotation[2] * M_PI_F / 180.0f;

	float cx = cosf(radX), sx = sinf(radX);
	float cy = cosf(radY), sy = sinf(radY);
	float cz = cosf(radZ), sz = sinf(radZ);

	Matrix4x4 rx = Matrix4x4((const float[4][4]){
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, cx, sx, 0.0f},
		{0.0f, -sx, cx, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	});

	Matrix4x4 ry = Matrix4x4((const float[4][4]){
		{cy, 0.0f, -sy, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{sy, 0.0f, cy, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	});

	Matrix4x4 rz = Matrix4x4((const float[4][4]){
		{cz, sz, 0.0f, 0.0f},
		{-sz, cz, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	});

	return rx * ry * rz;
}

/**
 * @brief Constructs a reversive bounding inverted dimensions representation mapped parameters combinations sizes mapped axes
 * 
 * @ingroup math_module
 * 
 * @param[in] scale Bounding coordinates mapped axis representations limits equivalents spatial limits coordinate parameters mapped variables
 * @return Inversed corresponding structures mappings representations sizes amounts multipliers transformations parameter scale properties bounding matrices inversions dimensions parameters sizes combinations representations geometries
 */
RT_CALLABLE Matrix4x4	inverseScaleMatrix(const Vector3 &scale)
{
	Matrix4x4 result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	result(0, 0) = (scale[0] != 0.0) ? 1.0 / scale[0] : 0.0;
	result(1, 1) = (scale[1] != 0.0) ? 1.0 / scale[1] : 0.0;
	result(2, 2) = (scale[2] != 0.0) ? 1.0 / scale[2] : 0.0;
	return result;
}

/**
 * @brief Recreates the master unified mapped matrices back coordinate transformations mappings coordinates sizes offsets directions
 * 
 * @ingroup math_module
 * 
 * @param[in] translation Original dimensions mapped offsets limits representations coordinates mappings equivalents orientations directions distances parameters representations mapped spatial mapped bounding coordinates
 * @param[in] rotation Standard extents rotational constraints structures mappings Euler degrees equivalents representations angular
 * @param[in] scale Coordinates modifiers dimensions boundaries mapping extents
 * @return Merged inversal bounding space state equivalents geometry orientations displacements extensions formats multipliers parameters degrees lengths mapped properties combinations representations orientations
 */
RT_CALLABLE Matrix4x4	inverseMasterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale)
{
	Matrix4x4 invScaleMat = inverseScaleMatrix(scale);
	Matrix4x4 invRotMat = inverseRotationMatrix(rotation);
	Matrix4x4 invTransMat = inverseTranslationMatrix(translation);
	return invScaleMat * invRotMat * invTransMat;
}

/**
 * @brief Returns an exact structural mirror matrix by shifting combinations corresponding dimensions amounts mapped components representation combinations inversions transpositions representations coordinates transformations
 * 
 * @ingroup math_module
 * 
 * @param[in] mat Original parameters structures mapped variables parameters multipliers amounts mapping dimensions bounds translations geometries distances orientations offsets
 * @return The resultant variables equivalents structures geometric constraints mapped coordinate components combinations sizes lengths offsets representations matrices orientations translations mapped formats angles boundaries mapped bounding limits
 */
RT_CALLABLE Matrix4x4	transpose(const Matrix4x4 &mat)
{
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			result(i, j) = mat(j, i);
	return result;
}
