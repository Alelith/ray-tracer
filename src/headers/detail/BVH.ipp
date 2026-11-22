inline void BVH::build(std::vector<Primitive>& prims) {
	nodes.clear();
	if (prims.empty()) return;
	nodes.reserve(prims.size() * 2);
	buildRecursive(prims, 0, static_cast<uint32_t>(prims.size()));
}
inline bool BVH::closestHit(const std::vector<Primitive>& prims, const Ray& ray, float tMin, float tMax, float& tHit, uint32_t& primHit) const {
	if (nodes.empty()) return false;
	Vec3 invDir(1.0f / ray.dir.x, 1.0f / ray.dir.y, 1.0f / ray.dir.z);
	bool hit = false;
	float closest = tMax;

	uint32_t stack[64];
	int sp = 0;
	stack[sp++] = 0;
	while (sp > 0) {
		const BVHNode& node = nodes[stack[--sp]];
		if (!hitBox(node, ray.origin, invDir, closest)) continue;

		if (node.count > 0) { 
			for (uint32_t i = node.firstOrRight; i < node.firstOrRight + node.count; i++) {
				float t;
				if (intersectPrimitive(prims[i], ray, tMin, closest, t)) {
					hit = true;
					closest = t;
					primHit = i;
				}
			}
		}
		else {
			uint32_t nodeIndex = static_cast<uint32_t>(&node - nodes.data());
			uint32_t left = nodeIndex + 1, right = node.firstOrRight;
			bool negative = invDir[node.axis] < 0.0f;
			stack[sp++] = negative ? left : right;
			stack[sp++] = negative ? right : left;
		}
	}
	if (hit) tHit = closest;
	return hit;
}
inline bool BVH::anyHit(const std::vector<Primitive>& prims, const Ray& ray, float tMin, float tMax) const {
	if (nodes.empty()) return false;
	Vec3 invDir(1.0f / ray.dir.x, 1.0f / ray.dir.y, 1.0f / ray.dir.z);
	uint32_t stack[64];
	int sp = 0;
	stack[sp++] = 0;
	while (sp > 0) {
		const BVHNode& node = nodes[stack[--sp]];
		if (!hitBox(node, ray.origin, invDir, tMax)) continue;

		if (node.count > 0) { 
			for (uint32_t i = node.firstOrRight; i < node.firstOrRight + node.count; i++) {
				float t;
				if (intersectPrimitive(prims[i], ray, tMin, tMax, t)) return true;
			}
		}
		else {
			stack[sp++] = node.firstOrRight;
			stack[sp++] = static_cast<uint32_t>(&node - nodes.data()) + 1;
		}
	}
	return false;
}
inline size_t BVH::nodeCount() const { return nodes.size(); }

inline bool BVH::hitBox(const BVHNode& n, const Point3& o, const Vec3& invDir, float tMax) {
	float t0 = 0.0f, t1 = tMax;
	for (int a = 0; a < 3; a++) {
		float tNear = (n.bmin[a] - o[a]) * invDir[a];
		float tFar = (n.bmax[a] - o[a]) * invDir[a];
		if (tNear > tFar) std::swap(tNear, tFar);
		t0 = std::fmaxf(t0, tNear);
		t1 = std::fminf(t1, tFar);
		if (t1 < t0) return false;
	}
	return true;
}
inline uint32_t BVH::buildRecursive(std::vector<Primitive>& prims, uint32_t begin, uint32_t end) {
	uint32_t nodeIndex = static_cast<uint32_t>(nodes.size());
	nodes.emplace_back();

	AABB bounds, centroids;
	for (uint32_t i = begin; i < end; i++) {
		AABB b = boundsOf(prims[i]);
		bounds.grow(b);
		centroids.grow(b.centroid());
	}
	nodes[nodeIndex].bmin = bounds.min;
	nodes[nodeIndex].bmax = bounds.max;

	const uint32_t count = end - begin;
	Vec3 ext = centroids.extent();
	int axis = (ext.x > ext.y && ext.x > ext.z) ? 0 : (ext.y > ext.z ? 1 : 2);

	constexpr uint32_t kLeafSize = 4;
	if (count <= kLeafSize || ext[axis] <= 0.0f) {
		nodes[nodeIndex].firstOrRight = begin;
		nodes[nodeIndex].count = static_cast<uint16_t>(count);
		nodes[nodeIndex].axis = 0;
		return nodeIndex;
	}

	uint32_t mid = begin + count / 2;
	std::nth_element(prims.begin() + begin, prims.begin() + mid, prims.begin() + end,
	[axis](const Primitive& a, const Primitive& b) {
		return boundsOf(a).centroid()[axis] < boundsOf(b).centroid()[axis];
	});

	buildRecursive(prims, begin, mid);
	uint32_t right = buildRecursive(prims, mid, end);
	nodes[nodeIndex].firstOrRight = right;
	nodes[nodeIndex].count = 0;
	nodes[nodeIndex].axis = static_cast<uint16_t>(axis);
	return nodeIndex;
}
