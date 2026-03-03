#ifndef TRANSFORM_HPP
# define TRANSFORM_HPP

# include "Matrix4x4.hpp"

class Transform
{
	private:
		Matrix4x4	matrix;
		Matrix4x4	inverseMatrix;
		Matrix4x4	transposeMatrix;
	public:
		Transform();
		Transform(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);
		Transform(const Transform &other);
		~Transform();

		Transform	&operator=(const Transform &other);

		Matrix4x4	getMatrix() const;
		Matrix4x4	getInverseMatrix() const;
		Matrix4x4	getTransposeMatrix() const;
};

std::ostream	&operator<<(std::ostream &os, const Transform &transform);

#endif /* TRANSFORM_HPP */
