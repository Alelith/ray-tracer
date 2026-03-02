#include "Geometry.hpp"

Cone::Cone(double radius, double height, Point3 &center, Vector3 &normal)
{ transform = Transform(center, normal, Vector3(radius, height, radius)); }

vector<double>	Cone::hit(const Ray &ray, Limits limits) const
{
	vector<double>	res;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	double	a = d[0] * d[0] + d[2] * d[2] - d[1] * d[1];
	double	b = 2 * (o[0] * d[0] + o[2] * d[2] - (o[1] - 0.5) * d[1]);
	double	c = o[0] * o[0] + o[2] * o[2] - (o[1] - 0.5) * (o[1] - 0.5);

	if (a > 1e-10)
	{
		vector<Solution>	sols = BhaskaraSolver(a, b, c).getSolutions();
		for (Solution sol : sols)
		{
			if (!sol.isComplex && limits.isBetween(sol.real))
			{
				double y = o[1] + sol.real * d[1];
				if (y >= -0.5 && y <= 0.5)
					res.emplace_back(sol.real);
			}
		}
	}

	if (fabs(d[1]) > 1e-10)
	{
		double t = (-0.5 - o[1]) / d[1];
		if (limits.isBetween(t))
		{
			double x = o[0] + t * d[0];
			double z = o[2] + t * d[2];
			if (x * x + z * z <= 1)
				res.emplace_back(t);
		}
	}

	return res;
}

Vector3	Cone::calculateNormal(Point3 p) const
{
	Point3	localP = transform.getInverseMatrix() * p;

	double radiusSq = localP[0] * localP[0] + localP[2] * localP[2];
	bool onBase = fabs(localP[1] + 0.5) < 1e-6 && radiusSq <= 1.0 + 1e-6;
	
	double expectedRadiusSq = (0.5 - localP[1]) * (0.5 - localP[1]);
	bool onBody = fabs(radiusSq - expectedRadiusSq) < 1e-6 && localP[1] >= -0.5 - 1e-6 && localP[1] <= 0.5 + 1e-6;

	if (!onBase && !onBody)
		return Vector3::Zero;

	Vector3	localNormal;
	if (onBase)
		localNormal = Vector3(0, -1, 0);
	else
		localNormal = Vector3(localP[0], 0.5 - localP[1], localP[2]).normalized();

	return (transform.getTransposeMatrix() * localNormal).normalized();
}
