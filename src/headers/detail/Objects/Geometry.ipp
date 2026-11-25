inline bool intersectSphere(const Point3& center, float radius, const Ray& ray, float tMin, float tMax, float& tHit) {
	Vec3 oc = ray.origin - center;
	float a = ray.dir.lengthSquared();
	float halfB = dot(oc, ray.dir);
	float c = oc.lengthSquared() - radius * radius;
	float disc = halfB * halfB - a * c;
	if (disc < 0.0f) return false;

	float sq = std::sqrtf(disc);
	float t = (-halfB - sq) / a;
	if (t <= tMin || t >= tMax) {
		t = (-halfB + sq) / a;
		if (t <= tMin || t >= tMax) return false;
	}
	tHit = t;
	return true;
}
inline bool intersectTriangle(const Point3& v0, const Point3& v1, const Point3& v2, const Ray& ray, float tMin, float tMax, float& tHit) {
	Vec3 e1 = v1 - v0;
	Vec3 e2 = v2 - v0;
	Vec3 pvec = cross(ray.dir, e2);
	float det = dot(e1, pvec);
	if (std::fabsf(det) < 1e-12f) return false;

	float invDet = 1.0f / det;
	Vec3 tvec = ray.origin - v0;
	float u = dot(tvec, pvec) * invDet;
	if (u < 0.0f || u > 1.0f) return false;

	Vec3 qvec = cross(tvec, e1);
	float v = dot(ray.dir, qvec) * invDet;
	if (v < 0.0f || u + v > 1.0f) return false;

	float t = dot(e2, qvec) * invDet;
	if (t <= tMin || t >= tMax) return false;
	tHit = t;
	return true;
}
inline bool intersectPrimitive(const Primitive& p, const Ray& ray, float tMin, float tMax, float& tHit) {
	if (p.type == PrimType::Sphere) return intersectSphere(p.a, p.radius, ray, tMin, tMax, tHit);
	return intersectTriangle(p.a, p.b, p.c, ray, tMin, tMax, tHit);
}

inline bool AABB::empty() const { return min.x > max.x; }
inline void AABB::grow(const Vec3& p) {
	min = {std::fminf(min.x, p.x), std::fminf(min.y, p.y), std::fminf(min.z, p.z)};
	max = {std::fmaxf(max.x, p.x), std::fmaxf(max.y, p.y), std::fmaxf(max.z, p.z)};
}	
inline void AABB::grow(const AABB& b) {
	if (b.empty()) return;
	grow(b.min);
	grow(b.max);
}
inline Vec3 AABB::centroid() const { return 0.5f * (min + max); }
inline Vec3 AABB::extent() const { return max - min; }

inline AABB boundsOf(const Primitive& p) {
	AABB box;
	if (p.type == PrimType::Sphere) {
		Vec3 r(p.radius, p.radius, p.radius);
		box.grow(p.a - r);
		box.grow(p.a + r);
	}
	else {
		box.grow(p.a);
		box.grow(p.b);
		box.grow(p.c);
	}
	return box;
}
