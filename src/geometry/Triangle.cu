#include "Geometry.cuh"

RT_CALLABLE void	makeTriangle(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, WorldGeometries &worldGeoms)
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
	int idx = worldGeoms.totalCount();

	size_t currentCount = worldGeoms.triangles.count;

	Point3* newV0 = new Point3[currentCount + 1];
	Point3* newV1 = new Point3[currentCount + 1];
	Point3* newV2 = new Point3[currentCount + 1];
	Vector3* newTriNormal = new Vector3[currentCount + 1];
	AABB* newAABB = new AABB[currentCount + 1];
	int* newGeomIdx = new int[currentCount + 1];

	for (size_t i = 0; i < currentCount; ++i)
	{
		newV0[i] = worldGeoms.triangles.v0[i];
		newV1[i] = worldGeoms.triangles.v1[i];
		newV2[i] = worldGeoms.triangles.v2[i];
		newTriNormal[i] = worldGeoms.triangles.triNormal[i];
		newAABB[i] = worldGeoms.triangles.boundingBox[i];
		newGeomIdx[i] = worldGeoms.triangles.geomIdx[i];
	}

	newV0[currentCount] = v0;
	newV1[currentCount] = v1;
	newV2[currentCount] = v2;
	newTriNormal[currentCount] = triNormal.normalized();
	newAABB[currentCount] = box;
	newGeomIdx[currentCount] = idx;

	delete[] worldGeoms.triangles.v0;
	delete[] worldGeoms.triangles.v1;
	delete[] worldGeoms.triangles.v2;
	delete[] worldGeoms.triangles.triNormal;
	delete[] worldGeoms.triangles.boundingBox;
	delete[] worldGeoms.triangles.geomIdx;

	worldGeoms.triangles.v0 = newV0;
	worldGeoms.triangles.v1 = newV1;
	worldGeoms.triangles.v2 = newV2;
	worldGeoms.triangles.triNormal = newTriNormal;
	worldGeoms.triangles.boundingBox = newAABB;
	worldGeoms.triangles.geomIdx = newGeomIdx;
	worldGeoms.triangles.count++;
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
