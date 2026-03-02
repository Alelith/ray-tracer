#include "Geometry.hpp"

Kiss::Kiss(double sharpness, Point3 &center, Vector3 &normal)
{
	transform = Transform(center, normal, Vector3::One);
	this->sharpness = sharpness;
}

vector<double>	Kiss::hit(const Ray &ray, Limits limits) const
{
	vector<double>	res;

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
			if (!sol.isComplex && limits.isBetween(sol.real))
			{
				double y = o[1] + sol.real * d[1];
				if (y >= -1.0 && y <= 1.0)
					res.emplace_back(sol.real);
			}
		}
	}

	return res;
}

Vector3	Kiss::calculateNormal(Point3 p) const
{
	Point3	localP = transform.getInverseMatrix() * p;

	double y2 = localP[1] * localP[1];
	double y4 = y2 * y2;
	double surfaceValue = localP[0] * localP[0] + localP[2] * localP[2]
						  - sharpness * y4 + (sharpness + 1.0) * y2 - 1.0;
	
	bool onSurface = fabs(surfaceValue) < 1e-6 && localP[1] >= -1.0 - 1e-6 && localP[1] <= 1.0 + 1e-6;
	
	if (!onSurface)
		return Vector3::Zero;

	Vector3 grad;
	grad[0] = 2.0 * localP[0];
	grad[1] = -sharpness * 4.0 * localP[1] * localP[1] * localP[1]
			  + (sharpness + 1.0) * 2.0 * localP[1];
	grad[2] = 2.0 * localP[2];

	Vector3 localNormal = grad.normalized();

	return (transform.getTransposeMatrix() * localNormal).normalized();
}
