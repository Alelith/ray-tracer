#include "Geometry.hpp"

Torus::Torus(double majorRadius, double minorRadius, Point3 &center, Vector3 &normal)
{
	transform = Transform(center, normal, Vector3(majorRadius, majorRadius, majorRadius));
	this->thickness = minorRadius / majorRadius;
}

vector<double>	Torus::hit(const Ray &ray, Limits limits) const
{
	vector<double>	res;

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
			if (!sol.isComplex && limits.isBetween(sol.real))
				res.emplace_back(sol.real);
	}

	return res;
}

Vector3	Torus::calculateNormal(Point3 p) const
{
	Point3	localP = transform.getInverseMatrix() * p;

	double rho = sqrt(localP[0] * localP[0] + localP[2] * localP[2]);
	if (rho < 1e-10)
		return Vector3::Zero;
	
	Point3 center(localP[0] / rho, 0, localP[2] / rho);
	Vector3 localNormal = (localP - center).normalized();

	return (transform.getTransposeMatrix() * localNormal).normalized();
}
