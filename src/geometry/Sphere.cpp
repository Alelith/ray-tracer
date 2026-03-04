#include "Geometry.hpp"

GeometryData	makeSphere(const Point3 &center, double radius)
{
	GeometryData g{};
	g.type      = GeometryType::Sphere;
	g.transform = Transform(center, Vector3::Up, Vector3(radius, radius, radius));
	return g;
}

bool	sphereHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	bool	wasHit = false;

	Ray		localRay = getLocalRay(g.transform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	double	a = dot(d, d);
	double	b = 2 * dot(d, o);
	double	c = dot(o, o) - 1;

	Solutions	sols = solveBhaskara(a, b, c);
	for (int i = 0; i < sols.count; ++i)
	{
		const Solution &sol = sols.data[i];
		if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
		{
			wasHit       = true;
			record.t     = sol.real;
			record.point = ray.at(sol.real);

			Point3	localHit    = o + sol.real * d;
			Vector3	localNormal = localHit.normalized();
			Vector3	worldNormal = (g.transform.getTransposeMatrix() * localNormal).normalized();
			record.setFaceNormal(ray, worldNormal);
		}
	}
	return wasHit;
}
