#ifndef HITTABLE_HPP
# define HITTABLE_HPP

# include "Ray.hpp"
# include "Solver.hpp"
# include "Transform.hpp"
# include "HitRecord.hpp"
# include "Limits.hpp"
# include <vector>

using std::vector;

class Geometry
{
	protected:
		Transform	transform;

		Ray			getLocalRay(const Ray &ray) const;
	public:
		virtual	~Geometry() = default;

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const = 0;
};

class Triangle : public Geometry
{
	private:
		Point3	v0, v1, v2;
		Vector3	normal;
	public:
		Triangle(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Plane : public Geometry
{
	public:
		Plane(const Point3 &point, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Sphere : public Geometry
{
	public:
		Sphere(const Point3 &center, double radius);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Cylinder : public Geometry
{
	public:
		Cylinder(double radius, double height, const Point3 &center, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Cone : public Geometry
{
	public:
		Cone(double radius, double height, const Point3 &center, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Hyperboloid : public Geometry
{
	private:
		double	waist;
	public:
		Hyperboloid(double waist, double radius, double height, const Point3 &center, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Torus : public Geometry
{
	private:
		double	thickness; // minorRadius / majorRadius
	public:
		Torus(double majorRadius, double minorRadius, const Point3 &center, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

class Kiss : public Geometry
{
	private:
		double	sharpness;
	public:
		Kiss(double sharpness, const Point3 &center, const Vector3 &normal);

		virtual HitRecord	hit(const Ray &ray, const Limits &limits) const override;
};

#endif /* HITTABLE_HPP */
