#include "Geometry.hpp"

Cylinder::Cylinder(double radius, double height, Point3 &center, Vector3 &normal)
{ transform = Transform(center, normal, Vector3(radius, height, radius)); }

HitRecord	Cylinder::hit(const Ray &ray, Limits limits) const
{
	HitRecord record;
	record.hit = false;
	record.t = INFINITY;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	// Intersection with cylinder body
	double	a = d[0] * d[0] + d[2] * d[2];
	double	b = 2 * (o[0] * d[0] + o[2] * d[2]);
	double	c = o[0] * o[0] + o[2] * o[2] - 1;

	if (a > 1e-10)
	{
		vector<Solution>	sols = BhaskaraSolver(a, b, c).getSolutions();
		for (Solution sol : sols)
		{
			if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
			{
				double y = o[1] + sol.real * d[1];
				if (y >= -0.5 && y <= 0.5)
				{
					record.hit = true;
					record.t = sol.real;
					record.point = ray.at(sol.real);
					
					Point3 localHitPoint = o + sol.real * d;
					Vector3 localNormal = Vector3(localHitPoint[0], 0, localHitPoint[2]).normalized();
					Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
					record.setFaceNormal(ray, worldNormal);
				}
			}
		}
	}

	// Intersection with bottom cap (y = -0.5)
	if (fabs(d[1]) > 1e-10)
	{
		double t = (-0.5 - o[1]) / d[1];
		if (limits.isBetween(t) && t < record.t)
		{
			double x = o[0] + t * d[0];
			double z = o[2] + t * d[2];
			if (x * x + z * z <= 1)
			{
				record.hit = true;
				record.t = t;
				record.point = ray.at(t);
				
				Vector3 localNormal = Vector3(0, -1, 0);
				Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
				record.setFaceNormal(ray, worldNormal);
			}
		}

		// Intersection with top cap (y = 0.5)
		t = (0.5 - o[1]) / d[1];
		if (limits.isBetween(t) && t < record.t)
		{
			double x = o[0] + t * d[0];
			double z = o[2] + t * d[2];
			if (x * x + z * z <= 1)
			{
				record.hit = true;
				record.t = t;
				record.point = ray.at(t);
				
				Vector3 localNormal = Vector3(0, 1, 0);
				Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
				record.setFaceNormal(ray, worldNormal);
			}
		}
	}

	return record;
}
