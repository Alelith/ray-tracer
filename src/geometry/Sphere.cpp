#include "Geometry.hpp"

void	makeSphere(const Point3 &center, float radius, WorldGeometries &worldGeoms)
{
	worldGeoms.spheres.inverseTransform.emplace_back(Transform(center, Vector3::Up, Vector3(radius, radius, radius)).getInverseMatrix());
	worldGeoms.spheres.transposeInverseTransform.emplace_back(Transform(center, Vector3::Up, Vector3(radius, radius, radius)).getTransposeInverseMatrix());
	worldGeoms.spheres.boundingBox.emplace_back(AABB(Point3(-1, -1, -1), Point3(1, 1, 1)));
}

GeometryData	makeSphere(const Point3 &center, float radius)
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

	float	a = dot(d, d);
	float	b = 2 * dot(d, o.toVector());
	float	c = dot(o.toVector(), o.toVector()) - 1;

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
			Vector3	localNormal = localHit.toVector().normalized();
			Vector3	worldNormal = (g.transform.getTransposeInverseMatrix() * localNormal).normalized();
			record.setFaceNormal(ray, worldNormal);
		}
	}
	return wasHit;
}
