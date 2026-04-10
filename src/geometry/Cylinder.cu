#include "Geometry.cuh"

RT_CALLABLE void	makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms)
{
	Matrix4x4	master = masterMatrix(center.toVector(), normal, Vector3(radius, height, radius));
	AABB		box(Point3(-1, -0.5, -1), Point3(1, 0.5, 1), master);
	int			idx = worldGeoms.totalCount();

	size_t currentCount = worldGeoms.cylinders.count;
	
	Matrix4x4* newInverse = new Matrix4x4[currentCount + 1];
	AABB* newAABB = new AABB[currentCount + 1];
	int* newGeomIdx = new int[currentCount + 1];

	for (size_t i = 0; i < currentCount; i++) {
		newInverse[i] = worldGeoms.cylinders.inverseTransform[i];
		newAABB[i] = worldGeoms.cylinders.boundingBox[i];
		newGeomIdx[i] = worldGeoms.cylinders.geomIdx[i];
	}

	newInverse[currentCount] = master.inverse();
	newAABB[currentCount] = box;
	newGeomIdx[currentCount] = idx;

	delete[] worldGeoms.cylinders.inverseTransform;
	delete[] worldGeoms.cylinders.boundingBox;
	delete[] worldGeoms.cylinders.geomIdx;

	worldGeoms.cylinders.inverseTransform = newInverse;
	worldGeoms.cylinders.boundingBox = newAABB;
	worldGeoms.cylinders.geomIdx = newGeomIdx;
	worldGeoms.cylinders.count++;
}

RT_CALLABLE bool	cylinderHits(const Matrix4x4 &invTransform, const Ray &ray, const Limits &limits, HitRecord &record)
{
	Ray		localRay = getLocalRay(invTransform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	float	bestT = limits.getMax();
	bool	found = false;
	Vector3	bestLocalNormal;

	// Lateral surface: x^2 + z^2 - 1 = 0 with y constrained to [-0.5, 0.5].
	float	a = d.x() * d.x() + d.z() * d.z();
	float	b = 2.0f * (o.x() * d.x() + o.z() * d.z());
	float	c = o.x() * o.x() + o.z() * o.z() - 1.0f;

	if (fabsf(a) > 1e-8f)
	{
		float	disc = b * b - 4.0f * a * c;
		if (disc >= 0.0f)
		{
			float	sqrtDisc = sqrtf(disc);
			float	t0 = (-b - sqrtDisc) / (2.0f * a);
			float	t1 = (-b + sqrtDisc) / (2.0f * a);

			if (limits.isBetween(t0))
			{
				float y0 = o.y() + t0 * d.y();
				if (y0 >= -0.5f && y0 <= 0.5f && t0 < bestT)
				{
					bestT = t0;
					found = true;
					Point3 p0 = o + t0 * d;
					bestLocalNormal = Vector3(p0.x(), 0.0f, p0.z()).normalized();
				}
			}

			if (limits.isBetween(t1))
			{
				float y1 = o.y() + t1 * d.y();
				if (y1 >= -0.5f && y1 <= 0.5f && t1 < bestT)
				{
					bestT = t1;
					found = true;
					Point3 p1 = o + t1 * d;
					bestLocalNormal = Vector3(p1.x(), 0.0f, p1.z()).normalized();
				}
			}
		}
	}

	// Caps at y = +/- 0.5.
	if (fabsf(d.y()) > 1e-8f)
	{
		float tTop = (0.5f - o.y()) / d.y();
		if (limits.isBetween(tTop) && tTop < bestT)
		{
			Point3 pTop = o + tTop * d;
			if (pTop.x() * pTop.x() + pTop.z() * pTop.z() <= 1.0f)
			{
				bestT = tTop;
				found = true;
				bestLocalNormal = Vector3(0.0f, 1.0f, 0.0f);
			}
		}

		float tBottom = (-0.5f - o.y()) / d.y();
		if (limits.isBetween(tBottom) && tBottom < bestT)
		{
			Point3 pBottom = o + tBottom * d;
			if (pBottom.x() * pBottom.x() + pBottom.z() * pBottom.z() <= 1.0f)
			{
				bestT = tBottom;
				found = true;
				bestLocalNormal = Vector3(0.0f, -1.0f, 0.0f);
			}
		}
	}

	if (!found)
		return false;

	record.t = bestT;
	record.point = ray.at(bestT);
	Vector3 worldNormal = (invTransform.transpose() * bestLocalNormal).normalized();
	record.setFaceNormal(ray, worldNormal);

	Point3 localHit = localRay.at(bestT);

	record.u = 0.5f + atan2f(bestLocalNormal.z(), bestLocalNormal.x()) / (2.0f * M_PI);
	record.v = localHit.y() + 0.5f;  // Map y from [-0.5, 0.5] to [0, 1]

	return true;
}
