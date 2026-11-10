inline bool intersectSphere(const Sphere& sphere, const Ray& ray, float tMin, float tMax, float& tHit) {
	Vec3 oc = ray.origin - sphere.center;
	float a = ray.dir.lengthSquared();
	float halfB = dot(oc, ray.dir);
	float c = oc.lengthSquared() - sphere.radius * sphere.radius;
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

inline uint32_t Scene::addMaterial(const Material& m) {
	materials.push_back(m);
	return static_cast<uint32_t>(materials.size() - 1);
}
inline uint32_t Scene::addSphere(Point3 center, float radius, uint32_t material) {
	spheres.push_back({center, radius, material});
	return static_cast<uint32_t>(spheres.size() - 1);
}
inline const Material Scene::materialOf(uint32_t prim) const { return materials[spheres[prim].material]; }
inline bool Scene::closestHit(const Ray& ray, float tMin, float tMax, float& tHit, uint32_t& primHit) const {
	bool hit = false;
	float closest = tMax;
	for (uint32_t i = 0; i < spheres.size(); i++)
	{
		float t;
		if (intersectSphere(spheres[i], ray, tMin, closest, t)) {
			hit = true;
			closest = t;
			primHit = i;
		}
	}
	if (hit) tHit = closest;
	return hit;
}
inline Vec3 Scene::outwardNormal(uint32_t prim, const Point3& p) const {
	const Sphere& s = spheres[prim];
	return (p - s.center) / s.radius;
}