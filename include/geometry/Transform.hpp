#ifndef TRANSFORM_HPP
# define TRANSFORM_HPP

# include "Matrix4x4.hpp"
# include "Point3.hpp"

class Transform
{
	private:
		Matrix4x4	matrix;
		Matrix4x4	inverseMatrix;
		Matrix4x4	transposeMatrix;
		Matrix4x4	transposeInverseMatrix;
	public:
		Transform();
		Transform(const Point3 &position, const Vector3 &rotation, const Vector3 &scale);
		Transform(const Transform &other) = default;
		~Transform() = default;

		Transform	&operator=(const Transform &other) = default;

		Matrix4x4	getMatrix() const;
		Matrix4x4	getInverseMatrix() const;
		Matrix4x4	getTransposeMatrix() const;
		Matrix4x4	getTransposeInverseMatrix() const;
};

std::ostream	&operator<<(std::ostream &os, const Transform &transform);

#endif /* TRANSFORM_HPP */
