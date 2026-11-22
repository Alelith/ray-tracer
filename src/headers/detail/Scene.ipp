inline uint32_t Scene::addMaterial(const Material& m) {
	materials.push_back(m);
	return static_cast<uint32_t>(materials.size() - 1);
}
inline void Scene::addSphere(Point3 center, float radius, uint32_t material) {
	Primitive p;
	p.type = PrimType::Sphere;
	p.material = material;
	p.a = center;
	p.radius = radius;
	prims.push_back(p);
}
inline void Scene::addTriangle(Point3 a, Point3 b, Point3 c, uint32_t material) {
	if (materials[material].type == MaterialType::Emissive)
		throw std::runtime_error("Lights cannot be meshes in this renderer");
	Primitive p;
	p.type = PrimType::Triangle;
	p.material = material;
	p.a = a;
	p.b = b;
	p.c = c;
	prims.push_back(p);
}
inline void Scene::addMesh(const Mesh& mesh, uint32_t material, float scale, Vec3 translate) {
	for (size_t i = 0; i + 2 < mesh.indexes.size(); i += 3)
		addTriangle(scale * mesh.vertex[mesh.indexes[i]] + translate,
					scale * mesh.vertex[mesh.indexes[i + 1]] + translate,
					scale * mesh.vertex[mesh.indexes[i + 2]] + translate, material);
}
inline void Scene::finalize(bool useBVH) {
	this->useBVH = useBVH;
	bvh.build(prims);
	lights.clear();
	for (uint32_t i = 0; i < prims.size(); i++)
		if (materials[prims[i].material].type == MaterialType::Emissive) lights.push_back(i);
}
inline Color Scene::sky(const Vec3& dir) const {
	float t = 0.5f * (dir.y + 1.0f);
	return lerp(skyHorizon, skyZenith, t);
}
inline const Material& Scene::materialOf(uint32_t prim) const { return materials[prims[prim].material]; }
inline const Primitive& Scene::primitive(uint32_t prim) const { return prims[prim]; }
inline const std::vector<uint32_t>& Scene::getLights() const { return lights; }
inline size_t Scene::primitiveCount() const { return prims.size(); }
inline size_t Scene::bvhNodeCount() const { return bvh.nodeCount(); }
inline bool Scene::closestHit(const Ray& ray, float tMin, float tMax, float& tHit, uint32_t& primHit) const {
	if (useBVH) return bvh.closestHit(prims, ray, tMin, tMax, tHit, primHit);

	bool hit = false;
	float closest = tMax;
	for (uint32_t i = 0; i < prims.size(); i++)
	{
		float t;
		if (intersectPrimitive(prims[i], ray, tMin, closest, t)) {
			hit = true;
			closest = t;
			primHit = i;
		}
	}
	if (hit) tHit = closest;
	return hit;
}
inline bool Scene::occluded(const Ray& ray, float tMin, float tMax) const {
	if (useBVH) return bvh.anyHit(prims, ray, tMin, tMax);
	for (const Primitive& p : prims) {
		float t;
		if (intersectPrimitive(p, ray, tMin, tMax, t)) return true;
	}
	return false;
}
inline Vec3 Scene::outwardNormal(uint32_t prim, const Point3& p) const {
	const Primitive& s = prims[prim];
	if (s.type == PrimType::Sphere) return (p - s.a) / s.radius;
	return cross(s.b - s.a, s.c - s.a).normalize();
}