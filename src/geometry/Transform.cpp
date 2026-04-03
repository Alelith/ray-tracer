#include "Transform.hpp"

Transform::Transform()
{
	matrix = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	inverseMatrix = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	transposeMatrix = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
	transposeInverseMatrix = Matrix4x4((const float[4][4]){{1.0, 0.0, 0.0, 0.0}, {0.0, 1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 0.0}, {0.0, 0.0, 0.0, 1.0}});
}

Transform::Transform(const Point3 &position, const Vector3 &rotation, const Vector3 &scale)
{
	Vector3 translation(position[0], position[1], position[2]);
	matrix = translationMatrix(translation) * rotationMatrix(rotation) * scaleMatrix(scale);
	inverseMatrix = inverseScaleMatrix(scale) * inverseRotationMatrix(rotation) * inverseTranslationMatrix(translation);
	transposeMatrix = transpose(matrix);
	transposeInverseMatrix = transpose(inverseMatrix);
}

Matrix4x4	Transform::getMatrix() const
{ return matrix; }

Matrix4x4	Transform::getInverseMatrix() const
{ return inverseMatrix; }

Matrix4x4	Transform::getTransposeMatrix() const
{ return transposeMatrix; }

Matrix4x4	Transform::getTransposeInverseMatrix() const
{ return transposeInverseMatrix; }

std::ostream	&operator<<(std::ostream &os, const Transform &transform)
{
	os << "Transform(" << transform.getMatrix() << ")" << std::endl;
	os << "Inverse: " << transform.getInverseMatrix() << std::endl;
	os << "Transpose: " << transform.getTransposeMatrix();
	return os;
}
