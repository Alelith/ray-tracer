#include "Transform.hpp"

Transform::Transform()
{
	matrix = Matrix4x4::Identity;
	inverseMatrix = Matrix4x4::Identity;
	transposeMatrix = Matrix4x4::Identity;
}

Transform::Transform(const Vector3 translation, const Vector3 rotation, const Vector3 scale)
{
	matrix = translationMatrix(translation) * rotationMatrix(rotation) * scaleMatrix(scale);
	inverseMatrix = inverseScaleMatrix(scale) * inverseRotationMatrix(rotation) * inverseTranslationMatrix(translation);
	transposeMatrix = transpose(matrix);
}

Transform::Transform(const Transform &other)
{
	matrix = other.matrix;
	inverseMatrix = other.inverseMatrix;
	transposeMatrix = other.transposeMatrix;
}

Transform::~Transform() {}

Transform	&Transform::operator=(const Transform &other)
{
	if (this != &other)
	{
		matrix = other.matrix;
		inverseMatrix = other.inverseMatrix;
		transposeMatrix = other.transposeMatrix;
	}
	return *this;
}

Matrix4x4	Transform::getMatrix() const
{ return matrix; }

Matrix4x4	Transform::getInverseMatrix() const
{ return inverseMatrix; }

Matrix4x4	Transform::getTransposeMatrix() const
{ return transposeMatrix; }

std::ostream	&operator<<(std::ostream &os, const Transform &transform)
{
	os << "Transform(" << transform.getMatrix() << ")" << std::endl;
	os << "Inverse: " << transform.getInverseMatrix() << std::endl;
	os << "Transpose: " << transform.getTransposeMatrix();
	return os;
}
