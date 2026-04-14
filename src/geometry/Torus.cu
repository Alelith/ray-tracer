#include "Geometry.cuh"
void	makeTorus(float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives)
{
	float		r = minorRadius / majorRadius;
	Matrix4x4	master = masterMatrix(center.toVector(), normal, Vector3(majorRadius, majorRadius, majorRadius));
	AABB		box(Point3(-(1 + r), -r, -(1 + r)), Point3(1 + r, r, 1 + r), master);
	Primitive	torus;
	torus.type = TORUS;
	torus.geomIdx = primitives.size();
	torus.boundingBox = box;
	torus.inverseTransform = master.inverse();
	torus.param = r;
	primitives.push_back(torus);
}
RT_CALLABLE bool	torusHits(const Matrix4x4 &invTransform, float param, const Ray &ray, const Limits &limits, HitRecord &record)
{
	Ray		localRay = getLocalRay(invTransform, ray);
	Point3	o = localRay.getOrigin();
	Vector3	d = localRay.getDirection();
	float	ox = o.x();
	float	oy = o.y();
	float	oz = o.z();
	float	dx = d.x();
	float	dy = d.y();
	float	dz = d.z();
	float	r2 = param * param;
	float	dd = dot(d, d);
	float	od = dot(o.toVector(), d);
	float	oo = dot(o.toVector(), o.toVector());
	float	dxz2 = dx * dx + dz * dz;
	float	oxz_dxz = ox * dx + oz * dz;
	float	oxz2 = ox * ox + oz * oz;
	float	A = dd;
	float	B = 2.0f * od;
	float	C = oo + 1.0f - r2;
	float	a = A * A;
	float	b = 2.0f * A * B;
	float	c = B * B + 2.0f * A * C - 4.0f * dxz2;
	float	dCoef = 2.0f * B * C - 8.0f * oxz_dxz;
	float	e = C * C - 4.0f * oxz2;
	Solutions roots = solveFerrari(a, b, c, dCoef, e);
	float	bestT = limits.getMax();
	bool	found = false;
	for (int i = 0; i < roots.count; ++i)
	{
		if (roots.data[i].isComplex)
			continue;
		float t = roots.data[i].real;
		if (limits.isBetween(t) && t < bestT)
		{
			bestT = t;
			found = true;
		}
	}
	if (!found)
		return false;
	record.t = bestT;
	record.point = ray.at(bestT);
	Point3	localHit = localRay.at(bestT);
	float	x = localHit.x();
	float	y = localHit.y();
	float	z = localHit.z();
	float	q = x * x + y * y + z * z + 1.0f - r2;
	Vector3	localNormal = Vector3(x * (q - 2.0f), y * q, z * (q - 2.0f)).normalized();
	Vector3	worldNormal = (invTransform.transpose() * localNormal).normalized();
	record.setFaceNormal(ray, worldNormal);
	float distance = sqrtf(x * x + z * z) - 1.0f;
	record.u = 0.5f + atan2f(localHit.z(), localHit.x()) / (2.0f * M_PI_F);
	record.v = 0.5f - atan2f(localHit.y(), distance) / (2.0 * M_PI_F);
	return true;
}