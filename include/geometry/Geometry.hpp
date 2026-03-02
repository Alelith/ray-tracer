#ifndef HITTABLE_HPP
# define HITTABLE_HPP

# include "Transform.hpp"
# include "Ray.hpp"
# include "Solver.hpp"
# include <vector>

using std::vector;

struct Limits
{
	double tMin;
	double tMax;

	Limits(double tMin = 0.001, double tMax = INFINITY);
	
	bool	isBetween(double n);
};

class Geometry
{
	protected:
		Transform	transform;

		Ray			getLocalRay(const Ray &ray) const;
	public:
		virtual	~Geometry() = default;

		virtual vector<double>	hit(const Ray &ray, Limits limits) const = 0;
		virtual Vector3			calculateNormal(Point3 p) const = 0;
};

class Triangle : public Geometry
{
	private:
		Point3	v0, v1, v2;
		Vector3	normal;
	public:
		Triangle(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Plane : public Geometry
{
	public:
		Plane(const Point3 &point, const Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Sphere : public Geometry
{
	public:
		Sphere(const Point3 &center, double radius);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Cylinder : public Geometry
{
	public:
		Cylinder(double radius, double height, Point3 &center, Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Cone : public Geometry
{
	public:
		Cone(double radius, double height, Point3 &center, Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Hyperboloid : public Geometry
{
	private:
		double	waist;
	public:
		Hyperboloid(double waist, double radius, double height, Point3 &center, Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Torus : public Geometry
{
	private:
		double	thickness; // minorRadius / majorRadius
	public:
		Torus(double majorRadius, double minorRadius, Point3 &center, Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

class Kiss : public Geometry
{
	private:
		double	sharpness;
	public:
		Kiss(double sharpness, Point3 &center, Vector3 &normal);

		virtual vector<double>	hit(const Ray &ray, Limits limits) const override;
		virtual Vector3			calculateNormal(Point3 p) const override;
};

#endif /* HITTABLE_HPP */
