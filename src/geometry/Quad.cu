#include "Geometry.cuh"

RT_CALLABLE void	makeQuad(const Point3 &Q, const Vector3 &u, const Vector3 &v, WorldGeometries &worldGeoms)
{
	Vector3 n = cross(u, v);
	Vector3 normal = n.normalized();
	float D = dot(normal, Q);
	Vector3 w = n / dot(n, n);

	// Compute bounding box
	Point3 p0 = Q;
	Point3 p1 = Q + u;
	Point3 p2 = Q + v;
	Point3 p3 = Q + u + v;

	Point3 minP(
		fminf(p0.x(), fminf(p1.x(), fminf(p2.x(), p3.x()))),
		fminf(p0.y(), fminf(p1.y(), fminf(p2.y(), p3.y()))),
		fminf(p0.z(), fminf(p1.z(), fminf(p2.z(), p3.z())))
	);
	Point3 maxP(
		fmaxf(p0.x(), fmaxf(p1.x(), fmaxf(p2.x(), p3.x()))),
		fmaxf(p0.y(), fmaxf(p1.y(), fmaxf(p2.y(), p3.y()))),
		fmaxf(p0.z(), fmaxf(p1.z(), fmaxf(p2.z(), p3.z())))
	);

	// Add a little padding to the bounding box to handle zero-thickness
	float pad = 0.0001f;
	minP = minP - Vector3(pad, pad, pad);
	maxP = maxP + Vector3(pad, pad, pad);

	AABB box(minP, maxP);
	int idx = worldGeoms.totalCount();

	size_t currentCount = worldGeoms.quads.count;

	Point3* newQ = new Point3[currentCount + 1];
	Vector3* newU = new Vector3[currentCount + 1];
	Vector3* newV = new Vector3[currentCount + 1];
	Vector3* newW = new Vector3[currentCount + 1];
	Vector3* newQuadNormal = new Vector3[currentCount + 1];
	float* newD = new float[currentCount + 1];
	AABB* newAABB = new AABB[currentCount + 1];
	int* newGeomIdx = new int[currentCount + 1];

	for (size_t i = 0; i < currentCount; ++i)
	{
		newQ[i] = worldGeoms.quads.Q[i];
		newU[i] = worldGeoms.quads.u[i];
		newV[i] = worldGeoms.quads.v[i];
		newW[i] = worldGeoms.quads.w[i];
		newQuadNormal[i] = worldGeoms.quads.quadNormal[i];
		newD[i] = worldGeoms.quads.D[i];
		newAABB[i] = worldGeoms.quads.boundingBox[i];
		newGeomIdx[i] = worldGeoms.quads.geomIdx[i];
	}

	newQ[currentCount] = Q;
	newU[currentCount] = u;
	newV[currentCount] = v;
	newW[currentCount] = w;
	newQuadNormal[currentCount] = normal;
	newD[currentCount] = D;
	newAABB[currentCount] = box;
	newGeomIdx[currentCount] = idx;

	delete[] worldGeoms.quads.Q;
	delete[] worldGeoms.quads.u;
	delete[] worldGeoms.quads.v;
	delete[] worldGeoms.quads.w;
	delete[] worldGeoms.quads.quadNormal;
	delete[] worldGeoms.quads.D;
	delete[] worldGeoms.quads.boundingBox;
	delete[] worldGeoms.quads.geomIdx;

	worldGeoms.quads.Q = newQ;
	worldGeoms.quads.u = newU;
	worldGeoms.quads.v = newV;
	worldGeoms.quads.w = newW;
	worldGeoms.quads.quadNormal = newQuadNormal;
	worldGeoms.quads.D = newD;
	worldGeoms.quads.boundingBox = newAABB;
	worldGeoms.quads.geomIdx = newGeomIdx;
	worldGeoms.quads.count++;
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
