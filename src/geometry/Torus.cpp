#include "Geometry.hpp"

Torus::Torus(double majorRadius, double minorRadius, const Point3 &center, const Vector3 &normal)
{
	transform = Transform(center, normal, Vector3(majorRadius, majorRadius, majorRadius));
	this->thickness = minorRadius / majorRadius;
}

HitRecord	Torus::hit(const Ray &ray, const Limits &limits) const
{
	HitRecord record;
	record.hit = false;
	record.t = INFINITY;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	dir = localRay.getDirection();

	double	dd = dot(dir, dir);
	double	od = dot(o, dir);
	double	oo = dot(o, o);
	double	sumR2 = 1 + thickness * thickness;
	double	diffR2 = 1 - thickness * thickness;

	double	a = dd * dd;
	double	b = 4 * dd * od;
	double	c = 2 * dd * (oo - sumR2) + 4 * od * od + 4 * dir[1] * dir[1];
	double	d = 4 * od * (oo - sumR2) + 8 * dir[1] * o[1];
	double	e = (oo + diffR2) * (oo + diffR2) - 4 * (oo - o[1] * o[1]);

	if (a > 1e-10)
	{
		vector<Solution>	sols = FerrariSolver(a, b, c, d, e).getSolutions();
		for (Solution sol : sols)
		{
			if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
			{
				record.hit = true;
				record.t = sol.real;
				record.point = ray.at(sol.real);
				
				Point3 localHitPoint = o + sol.real * dir;
				double rho = sqrt(localHitPoint[0] * localHitPoint[0] + localHitPoint[2] * localHitPoint[2]);
				Point3 center(localHitPoint[0] / rho, 0, localHitPoint[2] / rho);
				Vector3 localNormal = (localHitPoint - center).normalized();
				Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
				record.setFaceNormal(ray, worldNormal);
			}
		}
	}

	return record;
}
