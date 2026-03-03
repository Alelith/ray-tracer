#include "Geometry.hpp"

Sphere::Sphere(const Point3 &center, double radius)
{ transform = Transform(center, Vector3::Up, Vector3(radius, radius, radius)); }

HitRecord	Sphere::hit(const Ray &ray, Limits limits) const
{
	HitRecord record;
	record.hit = false;
	record.t = INFINITY;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	double	a = dot(d, d);
	double	b = 2 * dot(d, o);
	double	c = dot(o, o) - 1;

	vector<Solution>	sols = BhaskaraSolver(a, b, c).getSolutions();

	for (Solution sol : sols)
	{
		if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
		{
			record.hit = true;
			record.t = sol.real;
			
			// Calculate hit point in local space
			Point3 localHitPoint = o + sol.real * d;
			
			// Calculate normal in local space
			Vector3 localNormal = localHitPoint.normalized();
			
			// Transform point and normal to world space
			record.point = ray.at(sol.real);
			Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
			record.setFaceNormal(ray, worldNormal);
		}
	}
	
	return record;
}
