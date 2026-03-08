#include "Geometry.hpp"

GeometryData	makeKiss(float sharpness, const Point3 &center, const Vector3 &normal)
{
	GeometryData g{};
	g.type      = GeometryType::Kiss;
	g.transform = Transform(center, normal, Vector3::One);
	g.sharpness = sharpness;
	return g;
}

bool	kissHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	bool	wasHit = false;

	Ray		localRay = getLocalRay(g.transform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	float	sh = g.sharpness;
	float	a  = -sh * d[1] * d[1] * d[1] * d[1];
	float	b  = -sh * 4.0 * o[1] * d[1] * d[1] * d[1];
	float	c  = d[0] * d[0] + d[2] * d[2]
			   - sh * 6.0 * o[1] * o[1] * d[1] * d[1]
			   + (sh + 1.0) * d[1] * d[1];
	float	d_coef = 2.0 * o[0] * d[0] + 2.0 * o[2] * d[2]
				   - sh * 4.0 * o[1] * o[1] * o[1] * d[1]
				   + (sh + 1.0) * 2.0 * o[1] * d[1];
	float	e  = o[0] * o[0] + o[2] * o[2]
			   - sh * o[1] * o[1] * o[1] * o[1]
			   + (sh + 1.0) * o[1] * o[1] - 1.0;

	if (a > 1e-10)
	{
		Solutions	sols = solveFerrari(a, b, c, d_coef, e);
		for (int i = 0; i < sols.count; ++i)
		{
			const Solution &sol = sols.data[i];
			if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
			{
				float y = o[1] + sol.real * d[1];
				if (y >= -1.0 && y <= 1.0)
				{
					wasHit       = true;
					record.t     = sol.real;
					record.point = ray.at(sol.real);

					Point3	localHit = o + sol.real * d;
					float	y2 = localHit[1] * localHit[1];
					Vector3	grad;
					grad[0] = 2.0 * localHit[0];
					grad[1] = -sh * 4.0 * localHit[1] * y2 + (sh + 1.0) * 2.0 * localHit[1];
					grad[2] = 2.0 * localHit[2];

					Vector3 localNormal = grad.normalized();
					Vector3 worldNormal = (g.transform.getTransposeMatrix() * localNormal).normalized();
					record.setFaceNormal(ray, worldNormal);
				}
			}
		}
	}
	return wasHit;
}
