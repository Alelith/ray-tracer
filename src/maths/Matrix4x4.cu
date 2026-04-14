#include "Matrix4x4.cuh"

namespace maths
{

	RT_CALLABLE Matrix4x4::Matrix4x4() : m{{0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 1.0}} {}
	RT_CALLABLE Matrix4x4::Matrix4x4(const float values[4][4])
	{	
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				m[i][j] = values[i][j];
	}

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

	RT_CALLABLE Matrix4x4	translationMatrix(const Vector3 &translation)
	{
		Matrix4x4	result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
		result(0, 3) = translation[0];
		result(1, 3) = translation[1];
		result(2, 3) = translation[2];
		return result;
	}

	RT_CALLABLE Matrix4x4	rotationMatrix(const Vector3 &rotation)
	{
		float	radX = rotation[0] * M_PI_F / 180.0f;
		float	radY = rotation[1] * M_PI_F / 180.0f;
		float	radZ = rotation[2] * M_PI_F / 180.0f;

		float	cx = cosf(radX), sx = sinf(radX);
		float	cy = cosf(radY), sy = sinf(radY);
		float	cz = cosf(radZ), sz = sinf(radZ);

		Matrix4x4	rx = Matrix4x4((const float[4][4]){{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, cx, -sx, 0.0f}, {0.0f, sx, cx, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
		Matrix4x4	ry = Matrix4x4((const float[4][4]){{cy, 0.0f, sy, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {-sy, 0.0f, cy, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
		Matrix4x4	rz = Matrix4x4((const float[4][4]){{cz, -sz, 0.0f, 0.0f}, {sz, cz, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
		
		return rz * ry * rx;
	}

	RT_CALLABLE Matrix4x4	scaleMatrix(const Vector3 &scale)
	{
		Matrix4x4	result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});

		result(0, 0) = scale[0];
		result(1, 1) = scale[1];
		result(2, 2) = scale[2];

		return result;
	}

	RT_CALLABLE Matrix4x4	masterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale)
	{
		Matrix4x4	transMat = translationMatrix(translation);
		Matrix4x4	rotMat   = rotationMatrix(rotation);
		Matrix4x4	scaleMat = scaleMatrix(scale);
		return transMat * rotMat * scaleMat;
	}

	RT_CALLABLE Matrix4x4	inverseTranslationMatrix(const Vector3 &translation)
	{
		Matrix4x4	result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});

		result(0, 3) = -translation[0];
		result(1, 3) = -translation[1];
		result(2, 3) = -translation[2];

		return result;
	}

	RT_CALLABLE Matrix4x4	inverseRotationMatrix(const Vector3 &rotation)
	{
		float	radX = rotation[0] * M_PI_F / 180.0f;
		float	radY = rotation[1] * M_PI_F / 180.0f;
		float	radZ = rotation[2] * M_PI_F / 180.0f;

		float	cx = cosf(radX), sx = sinf(radX);
		float	cy = cosf(radY), sy = sinf(radY);
		float	cz = cosf(radZ), sz = sinf(radZ);

		Matrix4x4	rx = Matrix4x4((const float[4][4]){{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, cx, sx, 0.0f}, {0.0f, -sx, cx, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
		Matrix4x4	ry = Matrix4x4((const float[4][4]){{cy, 0.0f, -sy, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {sy, 0.0f, cy, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
		Matrix4x4	rz = Matrix4x4((const float[4][4]){{cz, sz, 0.0f, 0.0f}, {-sz, cz, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});

		return rx * ry * rz;
	}

	RT_CALLABLE Matrix4x4	inverseScaleMatrix(const Vector3 &scale)
	{
		Matrix4x4	result = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});

		result(0, 0) = (scale[0] != 0.0) ? 1.0 / scale[0] : 0.0;
		result(1, 1) = (scale[1] != 0.0) ? 1.0 / scale[1] : 0.0;
		result(2, 2) = (scale[2] != 0.0) ? 1.0 / scale[2] : 0.0;

		return result;
	}

	RT_CALLABLE Matrix4x4	inverseMasterMatrix(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale)
	{
		Matrix4x4	invScaleMat = inverseScaleMatrix(scale);
		Matrix4x4	invRotMat   = inverseRotationMatrix(rotation);
		Matrix4x4	invTransMat = inverseTranslationMatrix(translation);
		return invScaleMat * invRotMat * invTransMat;
	}

	RT_CALLABLE Matrix4x4	transpose(const Matrix4x4 &mat)
	{
		Matrix4x4	result;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				result(i, j) = mat(j, i);
		return result;
	}

} // namespace maths
