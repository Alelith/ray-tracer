#include "Geometry.hpp"

Triangle::Triangle(const Point3 &vertex0, const Point3 &vertex1, const Point3 &vertex2, const Vector3 &n)
{
	this->v0 = vertex0;
	this->v1 = vertex1;
	this->v2 = vertex2;
	this->normal = n;
}

static bool	isInsideTriangle(const Point3 &v0, const Point3 &v1, const Point3 &v2,
							const Vector3 &normal, const Point3 &hitPoint)
{
	Vector3 edge, vp, cp;

	edge = v1 - v0;
	vp = hitPoint - v0;
	cp = cross(edge, vp);
	if (dot(normal, cp) < 0)
		return false;

	edge = v2 - v1;
	vp = hitPoint - v1;
	cp = cross(edge, vp);
	if (dot(normal, cp) < 0)
		return false;

	edge = v0 - v2;
	vp = hitPoint - v2;
	cp = cross(edge, vp);
	if (dot(normal, cp) < 0)
		return false;

	return true;
}

vector<double>	Triangle::hit(const Ray &ray, Limits limits) const
{
	vector<double>	res;

	Vector3	dir = ray.getDirection();
	Point3	orig = ray.getOrigin();

	double denom = dot(normal, dir);
	if (fabs(denom) < 1e-6)
		return res;

	double t = dot(normal, v0 - orig) / denom;
	if (!limits.isBetween(t))
		return res;

	Point3 hitPoint = orig + t * dir;

	if (isInsideTriangle(v0, v1, v2, normal, hitPoint))
		res.emplace_back(t);

	return res;
}

Vector3	Triangle::calculateNormal(Point3 p) const
{
	double distance = fabs(dot(normal, p - v0));
	if (distance > 1e-6)
		return Vector3::Zero;

	if (!isInsideTriangle(v0, v1, v2, normal, p))
		return Vector3::Zero;

	return normal;
}
