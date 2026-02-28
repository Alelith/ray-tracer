#ifndef HITTABLE_HPP
# define HITTABLE_HPP

# include "Transform.hpp"
# include "Ray.hpp"
# include "HitRecord.hpp"

struct Limits
{
	double tMin;
	double tMax;

	Limits(double tMin = 0.001, double tMax = std::numeric_limits<double>::infinity())
		: tMin(tMin), tMax(tMax) {}
};

class Hittable
{
	protected:
		Transform				transform;
		shared_ptr<Material>	material;

		virtual Ray				applyInverseTransform(const Ray &ray) const;
	public:
		virtual	~Hittable() = default;

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const = 0;
};

class Triangle : public Hittable
{
	private:
		double	v0, v1, v2;
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Triangle(const Point3 &v0, const Point3 &v1, const Point3 &v2);
		virtual ~Triangle();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Plane : public Hittable
{
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Plane(const Point3 &point, const Vector3 &normal);
		virtual ~Plane();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Sphere : public Hittable
{
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Sphere(const Point3 &center, double radius);
		virtual ~Sphere();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Cylinder : public Hittable
{
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Cylinder(double radius, double height, Point3 &center, Vector3 &normal);
		virtual ~Cylinder();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Cone : public Hittable
{
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Cone(double radius, double height, Point3 &center, Vector3 &normal);
		virtual ~Cone();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Hyperboloid : public Hittable
{
	private:
		double	waist;
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Hyperboloid(double waist, double radius, double height, Point3 &center, Vector3 &normal);
		virtual ~Hyperboloid();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Torus : public Hittable
{
	private:
		double	radRelation; // minorRadius / majorRadius
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Torus(double majorRadius, double minorRadius, Point3 &center, Vector3 &normal);
		virtual ~Torus();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

class Kiss : Hittable
{
	private:
		double	sharpness;
	protected:
		virtual Ray	applyInverseTransform(const Ray &ray) const override;
	public:
		Kiss(double sharpness, Point3 &center, Vector3 &normal);
		virtual ~Kiss();

		virtual bool hit(const Ray &ray, Limits limits, HitRecord &record) const override;
};

#endif /* HITTABLE_HPP */
