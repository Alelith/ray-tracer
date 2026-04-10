#include "Geometry.cuh"

RT_CALLABLE void	makeCone(float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms)
{
	Matrix4x4	master = masterMatrix(center.toVector(), normal, Vector3(radius, height, radius));
	AABB		box(Point3(-1, -0.5, -1), Point3(1, 0.5, 1), master);
	int			idx = worldGeoms.totalCount();

	size_t currentCount = worldGeoms.cones.count;

	Matrix4x4*	newInverse = new Matrix4x4[currentCount + 1];
	AABB*		newAABB = new AABB[currentCount + 1];
	int*			newGeomIdx = new int[currentCount + 1];

	for (size_t i = 0; i < currentCount; i++)
	{
		newInverse[i] = worldGeoms.cones.inverseTransform[i];
		newAABB[i] = worldGeoms.cones.boundingBox[i];
		newGeomIdx[i] = worldGeoms.cones.geomIdx[i];
	}

	newInverse[currentCount] = master.inverse();
	newAABB[currentCount] = box;
	newGeomIdx[currentCount] = idx;

	delete[] worldGeoms.cones.inverseTransform;
	delete[] worldGeoms.cones.boundingBox;
	delete[] worldGeoms.cones.geomIdx;

	worldGeoms.cones.inverseTransform = newInverse;
	worldGeoms.cones.boundingBox = newAABB;
	worldGeoms.cones.geomIdx = newGeomIdx;
	worldGeoms.cones.count++;
}

RT_CALLABLE bool	coneHits(const Matrix4x4 &invTransform, const Ray &ray, const Limits &limits, HitRecord &record)
{
	Ray		localRay = getLocalRay(invTransform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();

	float	bestT = limits.getMax();
	bool	found = false;
	Vector3	bestLocalNormal;

	// Lateral surface: x^2 + z^2 - (y - 0.5)^2 = 0 with y constrained to [-0.5, 0.5].
	float	a = d.x() * d.x() + d.z() * d.z() - d.y() * d.y();
	float	b = 2.0f * (o.x() * d.x() + o.z() * d.z() - (o.y() - 0.5f) * d.y());
	float	c = o.x() * o.x() + o.z() * o.z() - (o.y() - 0.5f) * (o.y() - 0.5f);

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
					bestLocalNormal = Vector3(p0.x(), -(p0.y() - 0.5f), p0.z()).normalized();
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
					bestLocalNormal = Vector3(p1.x(), -(p1.y() - 0.5f), p1.z()).normalized();
				}
			}
		}
	}
	else if (fabsf(b) > 1e-8f)
	{
		float t = -c / b;
		if (limits.isBetween(t))
		{
			float y = o.y() + t * d.y();
			if (y >= -0.5f && y <= 0.5f && t < bestT)
			{
				bestT = t;
				found = true;
				Point3 p = o + t * d;
				bestLocalNormal = Vector3(p.x(), -(p.y() - 0.5f), p.z()).normalized();
			}
		}
	}

	// Bottom cap at y = -0.5.
	if (fabsf(d.y()) > 1e-8f)
	{
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
	record.u = 0.5f + atan2f(localHit.z(), localHit.x()) / (2.0f * M_PI);
	record.v = localHit.y() + 0.5f;  // Map y from [-0.5, 0.5] to [0, 1]

	return true;
}
