#include "Geometry.hpp"

RT_CALLABLE void	makeSphere(const Point3 &center, float radius, WorldGeometries &worldGeoms)
{
	Matrix4x4	master = masterMatrix(center.toVector(), Vector3(0, 1, 0), Vector3(radius, radius, radius));
	AABB		box(Point3(-1, -1, -1), Point3(1, 1, 1), master);
	int			idx = worldGeoms.totalCount();

	size_t currentCount = worldGeoms.spheres.count;
	
	Matrix4x4* newInverse = new Matrix4x4[currentCount + 1];
	AABB* newAABB = new AABB[currentCount + 1];
	int* newGeomIdx = new int[currentCount + 1];

	for (size_t i = 0; i < currentCount; i++) {
		newInverse[i] = worldGeoms.spheres.inverseTransform[i];
		newAABB[i] = worldGeoms.spheres.boundingBox[i];
		newGeomIdx[i] = worldGeoms.spheres.geomIdx[i];
	}

	newInverse[currentCount] = master.inverse();
	newAABB[currentCount] = box;
	newGeomIdx[currentCount] = idx;

	delete[] worldGeoms.spheres.inverseTransform;
	delete[] worldGeoms.spheres.boundingBox;
	delete[] worldGeoms.spheres.geomIdx;

	worldGeoms.spheres.inverseTransform = newInverse;
	worldGeoms.spheres.boundingBox = newAABB;
	worldGeoms.spheres.geomIdx = newGeomIdx;
	worldGeoms.spheres.count++;
}

RT_CALLABLE bool	sphereHits(const Matrix4x4 &invTransform, const Ray &ray, const Limits &limits, HitRecord &record)
{
	Ray		localRay = getLocalRay(invTransform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	float	a = dot(d, d);
	float	half_b = dot(d, o.toVector());
	float	c = dot(o.toVector(), o.toVector()) - 1;

	float	discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return false;
	
	float	sqrtDisc = sqrt(discriminant);

	float	root = (-half_b - sqrtDisc) / a;
	if (!limits.isBetween(root))
	{
		root = (-half_b + sqrtDisc) / a;
		if (!limits.isBetween(root))
			return false;
	}

	record.t = root;
	record.point = ray.at(root);

	Point3	localHit = o + root * d;
	Vector3	localNormal = localHit.toVector().normalized();
	Vector3	worldNormal = (invTransform.transpose() * localNormal).normalized();
	record.setFaceNormal(ray, worldNormal);
	
	return true;
}
