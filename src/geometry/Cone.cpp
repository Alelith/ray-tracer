#include "Geometry.hpp"

GeometryData	makeCone(float radius, float height, const Point3 &center, const Vector3 &normal)
{
	GeometryData g{};
	g.type      = GeometryType::Cone;
	g.transform = Transform(center, normal, Vector3(radius, height, radius));
	return g;
}

bool	coneHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	bool	wasHit = false;

	Ray		localRay = getLocalRay(g.transform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	float	a = d[0] * d[0] + d[2] * d[2] - d[1] * d[1];
	float	b = 2 * (o[0] * d[0] + o[2] * d[2] - (o[1] - 0.5) * d[1]);
	float	c = o[0] * o[0] + o[2] * o[2] - (o[1] - 0.5) * (o[1] - 0.5);

	if (a > 1e-10)
	{
		Solutions	sols = solveBhaskara(a, b, c);
		for (int i = 0; i < sols.count; ++i)
		{
			const Solution &sol = sols.data[i];
			if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
			{
				float y = o[1] + sol.real * d[1];
				if (y >= -0.5 && y <= 0.5)
				{
					wasHit       = true;
					record.t     = sol.real;
					record.point = ray.at(sol.real);

					Point3	localHit    = o + sol.real * d;
					Vector3	localNormal = Vector3(localHit[0], 0.5 - localHit[1], localHit[2]).normalized();
					Vector3	worldNormal = (g.transform.getTransposeInverseMatrix() * localNormal).normalized();
					record.setFaceNormal(ray, worldNormal);
				}
			}
		}
	}

	if (fabs(d[1]) > 1e-10)
	{
		float t = (-0.5 - o[1]) / d[1];
		if (limits.isBetween(t) && t < record.t)
		{
			float x = o[0] + t * d[0];
			float z = o[2] + t * d[2];
			if (x * x + z * z <= 1)
			{
				wasHit       = true;
				record.t     = t;
				record.point = ray.at(t);
			Vector3 worldNormal = (g.transform.getTransposeInverseMatrix() * Vector3(0, -1, 0)).normalized();
				record.setFaceNormal(ray, worldNormal);
			}
		}
	}
	return wasHit;
}
