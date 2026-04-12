#include "Geometry.cuh"

void	makeQuad(const Point3 &Q, const Vector3 &u, const Vector3 &v, PrimitivesList &primitives)
{
	Vector3 n = cross(u, v);
	Vector3 normal = n.normalized();
	float D = dot(normal, Q);
	Vector3 w = n / dot(n, n);

	Point3 p0 = Q;
	Point3 p1 = Q + u;
	Point3 p2 = Q + u + v;
	Point3 p3 = Q + v;

	float padding = 1e-4f;
	Point3 minP(
		fminf(p0.x(), fminf(p1.x(), fminf(p2.x(), p3.x()))) - padding,
		fminf(p0.y(), fminf(p1.y(), fminf(p2.y(), p3.y()))) - padding,
		fminf(p0.z(), fminf(p1.z(), fminf(p2.z(), p3.z()))) - padding
	);
	Point3 maxP(
		fmaxf(p0.x(), fmaxf(p1.x(), fmaxf(p2.x(), p3.x()))) + padding,
		fmaxf(p0.y(), fmaxf(p1.y(), fmaxf(p2.y(), p3.y()))) + padding,
		fmaxf(p0.z(), fmaxf(p1.z(), fmaxf(p2.z(), p3.z()))) + padding
	);

	Primitive quad;
	quad.type = QUAD;
	quad.geomIdx = primitives.size();
	quad.boundingBox = AABB(minP, maxP);

	quad.data.quad.Q = Q;
	quad.data.quad.u = u;
	quad.data.quad.v = v;
	quad.data.quad.w = w;
	quad.data.quad.normal = normal;
	quad.data.quad.D = D;

	primitives.push_back(quad);
}

RT_CALLABLE bool	quadHits(const Point3 &Q, const Vector3 &u, const Vector3 &v, const Vector3 &w, const Vector3 &normal, float D, const Ray &ray, const Limits &limits, HitRecord &record)
{
	float denom = dot(normal, ray.getDirection());

	// If the ray is parallel to the plane
	if (fabsf(denom) < 1e-8f)
		return false;

	// Calculate intersection t with the quad's plane
	float t = (D - dot(normal, ray.getOrigin())) / denom;
	if (!limits.isBetween(t))
		return false;

	// Determine if the hit point is within the quad
	Point3 intersection = ray.at(t);
	Vector3 planar_hitpt_vector = intersection - Q;
	float alpha = dot(w, cross(planar_hitpt_vector, v));
	float beta = dot(w, cross(u, planar_hitpt_vector));

	if (alpha < 0.0f || alpha > 1.0f || beta < 0.0f || beta > 1.0f)
		return false;

	record.t = t;
	record.point = intersection;
	record.setFaceNormal(ray, normal);
	record.u = alpha;
	record.v = beta;

	return true;
}
