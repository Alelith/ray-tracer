#include "Geometry.hpp"

GeometryData	makePlane(const Point3 &point, const Vector3 &normal)
{
	GeometryData g{};
	g.type      = GeometryType::Plane;
	g.transform = Transform(point, normal, Vector3::One);
	return g;
}

bool	planeHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	bool	wasHit = false;

	Ray		localRay = getLocalRay(g.transform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	if (fabs(d[1]) > 1e-10)
	{
		double t = -o[1] / d[1];
		if (limits.isBetween(t) && t < record.t)
		{
			wasHit       = true;
			record.t     = t;
			record.point = ray.at(t);

			Vector3 localNormal = Vector3(0, 1, 0);
			Vector3 worldNormal = (g.transform.getTransposeMatrix() * localNormal).normalized();
			record.setFaceNormal(ray, worldNormal);
		}
	}
	return wasHit;
}
