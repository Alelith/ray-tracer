#include "Geometry.hpp"

Plane::Plane(const Point3 &point, const Vector3 &normal)
{ transform = Transform(point, normal, Vector3::One); }

vector<double>	Plane::hit(const Ray &ray, Limits limits) const
{
	vector<double>	res;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	if (fabs(d[1]) > 1e-10)
	{
		double t = -o[1] / d[1];
		if (limits.isBetween(t))
			res.emplace_back(t);
	}

	return res;
}

Vector3	Plane::calculateNormal(Point3 p) const
{
	Point3	localP = transform.getInverseMatrix() * p;

	if (fabs(localP[1]) > 1e-6)
		return Vector3::Zero;

	Vector3	localNormal = Vector3(0, 1, 0);

	return (transform.getTransposeMatrix() * localNormal).normalized();
}


