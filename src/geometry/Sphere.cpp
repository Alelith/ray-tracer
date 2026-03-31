#include "Geometry.hpp"

void	makeSphere(const Point3 &center, float radius, WorldGeometries &worldGeoms)
{
	Transform	t = Transform(center, Vector3::Up, Vector3(radius, radius, radius));
	AABB		box(Point3(-1, -1, -1), Point3(1, 1, 1), t);
	int			idx = worldGeoms.totalCount();  // Calculate index BEFORE adding
	worldGeoms.spheres.inverseTransform.emplace_back(t.getInverseMatrix());
	worldGeoms.spheres.transposeInverseTransform.emplace_back(t.getTransposeInverseMatrix());
	worldGeoms.spheres.boundingBox.emplace_back(box);
	worldGeoms.spheres.geomIdx.emplace_back(idx);
}

bool	sphereHits(const Matrix4x4 &invTransform, const Ray &ray, const Limits &limits, HitRecord &record)
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
	Vector3	worldNormal = (transpose(invTransform) * localNormal).normalized();
	record.setFaceNormal(ray, worldNormal);
	
	return true;
}

bool	spheresHit(const SimpleGeometryData &spheres, const Ray &ray, Limits &limits, HitRecord &record)
{
	HitRecord	tmpRecord;
	bool		hitAnything = false;

	for (size_t i = 0; i < spheres.inverseTransform.size(); ++i)
	{
		if (sphereHits(spheres.inverseTransform[i], ray, limits, tmpRecord))
		{
			hitAnything = true;
			record = tmpRecord;
			record.geomIdx = spheres.geomIdx[i]; // Store the index of the hit geometry
			limits = Limits(limits.getMin(), record.t); // Update limits to find closest hit
		}
	}

	return hitAnything;
}
