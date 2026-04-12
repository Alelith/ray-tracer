#include "Geometry.cuh"

void	makeTriangle(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, PrimitivesList &primitives)
{
	Vector3 triNormal = normal;
	if (triNormal.isNearZero())
		triNormal = cross(v1 - v0, v2 - v0).normalized();

	Point3 minP(
		fminf(v0.x(), fminf(v1.x(), v2.x())),
		fminf(v0.y(), fminf(v1.y(), v2.y())),
		fminf(v0.z(), fminf(v1.z(), v2.z()))
	);
	Point3 maxP(
		fmaxf(v0.x(), fmaxf(v1.x(), v2.x())),
		fmaxf(v0.y(), fmaxf(v1.y(), v2.y())),
		fmaxf(v0.z(), fmaxf(v1.z(), v2.z()))
	);
	AABB box(minP, maxP);

	Primitive	triangle;
	triangle.type = TRIANGLE;
	triangle.geomIdx = primitives.size();
	triangle.boundingBox = box;
	triangle.data.tris.v0 = v0;
	triangle.data.tris.v1 = v1;
	triangle.data.tris.v2 = v2;
	triangle.data.tris.normal = triNormal.normalized();

	primitives.push_back(triangle);
}

RT_CALLABLE bool	triangleHits(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, const Ray &ray, const Limits &limits, HitRecord &record)
{
	const float eps = 1e-8f;

	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;

	Vector3 pvec = cross(ray.getDirection(), edge2);
	float det = dot(edge1, pvec);
	if (fabsf(det) < eps)
		return false;

	float invDet = 1.0f / det;
	Vector3 tvec = ray.getOrigin() - v0;
	float u = dot(tvec, pvec) * invDet;
	if (u < 0.0f || u > 1.0f)
		return false;

	Vector3 qvec = cross(tvec, edge1);
	float v = dot(ray.getDirection(), qvec) * invDet;
	if (v < 0.0f || (u + v) > 1.0f)
		return false;

	float t = dot(edge2, qvec) * invDet;
	if (!limits.isBetween(t))
		return false;

	record.t = t;
	record.point = ray.at(t);

	Vector3 outwardNormal = normal;
	if (outwardNormal.isNearZero())
		outwardNormal = cross(edge1, edge2);
	outwardNormal = outwardNormal.normalized();
	record.setFaceNormal(ray, outwardNormal);

	// Barycentric UVs for triangle texturing.
	record.u = u;
	record.v = v;

	return true;
}
