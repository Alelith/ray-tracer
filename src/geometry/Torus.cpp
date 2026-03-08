#include "Geometry.hpp"

GeometryData	makeTorus(double majorRadius, double minorRadius, const Point3 &center, const Vector3 &normal)
{
	GeometryData g{};
	g.type      = GeometryType::Torus;
	g.transform = Transform(center, normal, Vector3(majorRadius, majorRadius, majorRadius));
	g.thickness = minorRadius / majorRadius;
	return g;
}

bool	torusHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	bool	wasHit = false;

	Ray		localRay = getLocalRay(g.transform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	dir = localRay.getDirection();

	double	dd     = dot(dir, dir);
	double	od     = dot(o.toVector(), dir);
	double	oo     = dot(o.toVector(), o.toVector());
	double	sumR2  = 1 + g.thickness * g.thickness;
	double	diffR2 = 1 - g.thickness * g.thickness;

	double	a = dd * dd;
	double	b = 4 * dd * od;
	double	c = 2 * dd * (oo - sumR2) + 4 * od * od + 4 * dir[1] * dir[1];
	double	d = 4 * od * (oo - sumR2) + 8 * dir[1] * o[1];
	double	e = (oo + diffR2) * (oo + diffR2) - 4 * (oo - o[1] * o[1]);

	if (a > 1e-10)
	{
		Solutions	sols = solveFerrari(a, b, c, d, e);
		for (int i = 0; i < sols.count; ++i)
		{
			const Solution &sol = sols.data[i];
			if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
			{
				wasHit       = true;
				record.t     = sol.real;
				record.point = ray.at(sol.real);

				Point3	localHit = o + sol.real * dir;
				double	rho      = sqrt(localHit[0] * localHit[0] + localHit[2] * localHit[2]);
				Point3	center(localHit[0] / rho, 0, localHit[2] / rho);
				Vector3	localNormal = (localHit - center).normalized();
				Vector3	worldNormal = (g.transform.getTransposeMatrix() * localNormal).normalized();
				record.setFaceNormal(ray, worldNormal);
			}
		}
	}
	return wasHit;
}
