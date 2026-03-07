#include "Geometry.hpp"

GeometryData	makeTriangle(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal)
{
	GeometryData g{};
	g.type      = GeometryType::Triangle;
	g.v0        = v0;
	g.v1        = v1;
	g.v2        = v2;
	g.triNormal = normal;
	return g;
}

static bool	isInsideTriangle(const Point3 &v0, const Point3 &v1, const Point3 &v2,
							const Vector3 &normal, const Point3 &hitPoint)
{
	Vector3 edge, vp, cp;

	edge = v1 - v0;
	vp   = hitPoint - v0;
	cp   = cross(edge, vp);
	if (dot(normal, cp) < 0) return false;

	edge = v2 - v1;
	vp   = hitPoint - v1;
	cp   = cross(edge, vp);
	if (dot(normal, cp) < 0) return false;

	edge = v0 - v2;
	vp   = hitPoint - v2;
	cp   = cross(edge, vp);
	if (dot(normal, cp) < 0) return false;

	return true;
}

bool	triangleHit(const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	Vector3	dir  = ray.getDirection();
	Point3	orig = ray.getOrigin();

	double denom = dot(g.triNormal, dir);
	if (fabs(denom) < 1e-6)
		return false;

	double t = dot(g.triNormal, g.v0 - orig) / denom;
	if (!limits.isBetween(t) || t >= record.t)
		return false;

	Point3 hitPoint = orig + t * dir;
	if (isInsideTriangle(g.v0, g.v1, g.v2, g.triNormal, hitPoint))
	{
		record.t     = t;
		record.point = hitPoint;
		record.setFaceNormal(ray, g.triNormal);
		return true;
	}
	return false;
}
