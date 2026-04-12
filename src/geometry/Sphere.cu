#include "Geometry.cuh"

void	makeSphere(const Point3 &center, float radius, PrimitivesList &primitives)
{
	Matrix4x4	master = masterMatrix(center.toVector(), Vector3(0, 1, 0), Vector3(radius, radius, radius));
	AABB		box(Point3(-1, -1, -1), Point3(1, 1, 1), master);

	Primitive	prim;
	prim.type = SPHERE;
	prim.geomIdx = primitives.size();
	prim.boundingBox = box;
	prim.inverseTransform = master.inverse();

	primitives.push_back(prim);
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
	
	float	sqrtDisc = sqrtf(discriminant);

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

	record.u = 0.5 + atan2(localNormal.z(), localNormal.x()) / (2 * M_PI_F);
	record.v = 0.5 - asin(localNormal.y()) / M_PI_F;

	return true;
}
