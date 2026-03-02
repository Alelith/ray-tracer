#include "Geometry.hpp"

Sphere::Sphere(const Point3 &center, double radius)
{ transform = Transform(center, Vector3::Up, Vector3(radius, radius, radius)); }

vector<double>	Sphere::hit(const Ray &ray, Limits limits) const
{
	vector<double>	res;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	double	a = dot(d, d);
	double	b = 2 * dot(d, o);
	double	c = dot(o, o) - 1;

	vector<Solution>	sols = BhaskaraSolver(a, b, c).getSolutions();

	for (Solution sol : sols)
		if (!sol.isComplex && limits.isBetween(sol.real))
			res.emplace_back(sol.real);
	return res;
}

Vector3	Sphere::calculateNormal(Point3 p) const
{
	Point3	localP = transform.getInverseMatrix() * p;

	double distance = localP.length();
	if (fabs(distance - 1.0) > 1e-6)
		return Vector3::Zero;
	
	Vector3	localNormal = localP.normalized();
	
	return (transform.getTransposeMatrix() * localNormal).normalized();
}
