#include "Geometry.hpp"

Plane::Plane(const Point3 &point, const Vector3 &normal)
{ transform = Transform(point, normal, Vector3::One); }

HitRecord	Plane::hit(const Ray &ray, Limits limits) const
{
	HitRecord record;
	record.hit = false;
	record.t = INFINITY;

	Ray		localRay = getLocalRay(ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	if (fabs(d[1]) > 1e-10)
	{
		double t = -o[1] / d[1];
		if (limits.isBetween(t))
		{
			record.hit = true;
			record.t = t;
			record.point = ray.at(t);
			
			// Plane normal in local space is (0, 1, 0)
			Vector3 localNormal = Vector3(0, 1, 0);
			Vector3 worldNormal = (transform.getTransposeMatrix() * localNormal).normalized();
			record.setFaceNormal(ray, worldNormal);
		}
	}

	return record;
}
