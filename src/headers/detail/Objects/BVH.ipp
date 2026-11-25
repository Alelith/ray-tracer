inline void BVH::build(std::vector<Primitive>& prims) {
	nodes.clear();
	if (prims.empty()) return;
	
	const uint32_t n = static_cast<uint32_t>(prims.size());
	boxes.resize(n);
	centroids.resize(n);
	std::vector<uint32_t> order(n);
	for (uint32_t i = 0; i < n; i++) {
		boxes[i] = boundsOf(prims[i]);
		centroids[i] = boxes[i].centroid();
		order[i] = i;
	}
	nodes.reserve(2 * static_cast<size_t>(n));
	buildRecursive(order, 0, n);

	std::vector<Primitive> sorted(n);
	for (uint32_t i = 0; i < n; i++) sorted[i] = prims[order[i]];
	prims.swap(sorted);
	boxes.clear();
	centroids.clear();
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
inline float BVH::area(const AABB& b) {
	Vec3 e = b.extent();
	return 2.0f * (e.x * e.y + e.x * e.z + e.y * e.z);
}
inline uint32_t BVH::buildRecursive(std::vector<uint32_t>& order, uint32_t begin, uint32_t end) {
	uint32_t nodeIndex = static_cast<uint32_t>(nodes.size());
	nodes.emplace_back();

	AABB bounds, cb;
	for (uint32_t i = begin; i < end; i++) {
		bounds.grow(boxes[order[i]]);
		cb.grow(centroids[order[i]]);
	}
	nodes[nodeIndex].bmin = bounds.min;
	nodes[nodeIndex].bmax = bounds.max;

	const uint32_t count = end - begin;
	auto makeLeaf = [&]() {
		nodes[nodeIndex].firstOrRight = begin;
		nodes[nodeIndex].count = static_cast<uint16_t>(count);
		nodes[nodeIndex].axis = 0;
		return nodeIndex;
	};
	if (count <= 2) return makeLeaf();

	constexpr int kBins = 16;
	float bestCost = kInfinity;
	int bestAxis = -1;
	uint32_t bestSplit = 0;
	Vec3 extent = cb.extent();

	for (int axis = 0; axis < 3; axis++) {
		if (extent[axis] < 0.0f) continue;
		AABB binBoxs[kBins];
		uint32_t binCounts[kBins] = {};
		float scale = static_cast<float>(kBins) / extent[axis];
		for (uint32_t i = begin; i < end; i++) {
			uint32_t b = std::min(static_cast<uint32_t>(kBins - 1), static_cast<uint32_t>((centroids[order[i]][axis] - cb.min[axis]) * scale));
			binBoxs[b].grow(boxes[order[i]]);
			binCounts[b]++;
		}

		float leftAreas[kBins - 1], rightAreas[kBins - 1];
		uint32_t leftCounts[kBins - 1], rightCounts[kBins - 1];
		AABB acc;
		uint32_t countAcc = 0;
		for (int i = 0; i < kBins - 1; i++) {
			acc.grow(binBoxs[i]);
			countAcc += binCounts[i];
			leftAreas[i] = area(acc);
			leftCounts[i] = countAcc;
		}
		acc = AABB();
		countAcc = 0;
		for (int i = kBins - 1; i > 0; i--) {
			acc.grow(binBoxs[i]);
			countAcc += binCounts[i];
			rightAreas[i - 1] = area(acc);
			rightCounts[i - 1] = countAcc;
		}
		for (int i = 0; i < kBins - 1; i++) {
			if (leftCounts[i] == 0 || rightCounts[i] == 0) continue;
			float cost = leftAreas[i] * static_cast<float>(leftCounts[i]) + rightAreas[i] * static_cast<float>(rightCounts[i]);
			if (cost < bestCost) {
				bestCost = cost;
				bestAxis = axis;
				bestSplit = static_cast<uint32_t>(i);
			}
		}
	}

	float parentArea = area(bounds);
	if (bestAxis < 0) return makeLeaf();
	float splitCost = 0.5f + bestCost / parentArea;
	if (count <= 4 && splitCost >= static_cast<float>(count)) return makeLeaf();

	float scale = static_cast<float>(kBins) / extent[bestAxis];
	auto mid = std::partition(order.begin() + begin, order.begin() + end, [&](uint32_t i) {
		uint32_t b = std::min(static_cast<uint32_t>(kBins - 1), static_cast<uint32_t>((centroids[i][bestAxis] - cb.min[bestAxis]) * scale));
		return b <= bestSplit;
	});
	uint32_t midIndex = static_cast<uint32_t>(mid - order.begin());
	if (midIndex == begin || midIndex == end) return makeLeaf();

	buildRecursive(order, begin, midIndex);
	
	uint32_t right = buildRecursive(order, midIndex, end);
	nodes[nodeIndex].firstOrRight = right;
	nodes[nodeIndex].count = 0;
	nodes[nodeIndex].axis = static_cast<uint16_t>(bestAxis);
	return nodeIndex;
}
