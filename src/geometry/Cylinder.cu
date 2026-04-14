#include "Geometry.cuh"
void	makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives)
{
	Matrix4x4	master = masterMatrix(center.toVector(), normal, Vector3(radius, height, radius));
	AABB		box(Point3(-1, -0.5, -1), Point3(1, 0.5, 1), master);
	Primitive	prim;
	prim.type = CYLINDER;
	prim.geomIdx = primitives.size();
	prim.boundingBox = box;
	prim.inverseTransform = master.inverse();
	primitives.push_back(prim);
}
RT_CALLABLE bool	cylinderHits(const Matrix4x4 &invTransform, const Ray &ray, const Limits &limits, HitRecord &record)
{
	Ray		localRay = getLocalRay(invTransform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();
	float	bestT = limits.getMax();
	bool	found = false;
	Vector3	bestLocalNormal;
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
	record.u = 0.5f + atan2f(bestLocalNormal.z(), bestLocalNormal.x()) / (2.0f * M_PI_F);
	record.v = localHit.y() + 0.5f;  
	return true;
}