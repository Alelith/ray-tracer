#include "Geometry.hpp"

Kiss::Kiss(double sharpness, const Point3 &center, const Vector3 &normal)
{
	transform = Transform(center, normal, Vector3::One);
	this->sharpness = sharpness;
}

HitRecord	Kiss::hit(const Ray &ray, const Limits &limits) const
{
	HitRecord record;
	record.hit = false;
	record.t = INFINITY;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	double	a = -sharpness * d[1] * d[1] * d[1] * d[1];
	double	b = -sharpness * 4.0 * o[1] * d[1] * d[1] * d[1];
	double	c = d[0] * d[0] + d[2] * d[2]
				- sharpness * 6.0 * o[1] * o[1] * d[1] * d[1]
				+ (sharpness + 1.0) * d[1] * d[1];
	double	d_coef = 2.0 * o[0] * d[0] + 2.0 * o[2] * d[2]
				- sharpness * 4.0 * o[1] * o[1] * o[1] * d[1]
				+ (sharpness + 1.0) * 2.0 * o[1] * d[1];
	double	e = o[0] * o[0] + o[2] * o[2]
				- sharpness * o[1] * o[1] * o[1] * o[1]
				+ (sharpness + 1.0) * o[1] * o[1] - 1.0;

	if (a > 1e-10)
	{
		vector<Solution>	sols = FerrariSolver(a, b, c, d_coef, e).getSolutions();
		for (Solution sol : sols)
		{
			if (!sol.isComplex && limits.isBetween(sol.real) && sol.real < record.t)
			{
				double y = o[1] + sol.real * d[1];
				if (y >= -1.0 && y <= 1.0)
				{
					record.hit = true;
					record.t = sol.real;
					record.point = ray.at(sol.real);
					
					Point3 localHitPoint = o + sol.real * d;
					double y2 = localHitPoint[1] * localHitPoint[1];
					Vector3 grad;
					grad[0] = 2.0 * localHitPoint[0];
					grad[1] = -sharpness * 4.0 * localHitPoint[1] * y2
							  + (sharpness + 1.0) * 2.0 * localHitPoint[1];
					grad[2] = 2.0 * localHitPoint[2];
					
					Vector3 localNormal = grad.normalized();
					Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
					record.setFaceNormal(ray, worldNormal);
				}
			}
		}
	}

	return record;
}
