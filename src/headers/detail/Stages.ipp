#include "Parallel.hpp"
#include "RNG.hpp"

inline void RouteQueues::allocate(uint32_t capacity) {
	miss.allocate(capacity);
	for (auto& q : material) q.allocate(capacity);
}
inline void RouteQueues::clear() {
	miss.clear();
	for (auto& q : material) q.clear();
}

RouteBatch::RouteBatch(RouteQueues& t, uint32_t maxEntries) : target(t) { local.allocate(maxEntries); }
RouteBatch::~RouteBatch() {
	target.miss.appendBlock(local.miss);
	for (uint32_t i = 0; i < kMaterialCount; i++) target.material[i].appendBlock(local.material[i]);
}

inline void stageGenerateCamera(const Camera& cam, int width, int height, uint32_t sampleIndex, uint32_t firstPixel, uint32_t pixelCount, PathQueue& out) {
	out.setSize(pixelCount);
	parallelFor(pixelCount, [&](uint32_t k) {
		uint32_t pixelIndex = firstPixel + k;
		int x = static_cast<int>(pixelIndex) % width;
		int y = static_cast<int>(pixelIndex) / width;
		uint32_t rng = makeSeed(pixelIndex, sampleIndex);

		float s = (static_cast<float>(x) + rngNext(rng)) / static_cast<float>(width);
		float t = 1.0f - (static_cast<float>(y) + rngNext(rng)) / static_cast<float>(height);

		uint32_t slot = k;
		out.setRay(slot, cam.getRay(s, t, rng));
		out.pixel[slot] = pixelIndex;
		out.rng[slot] = rng;
		out.setThroughput(slot, Color(1, 1, 1));
		out.countEmission[slot] = 1;
	});
}
inline void stageIntersect(const Scene& scene, PathQueue& paths, RouteQueues& routes) {
	routes.clear();
	parallelForChunks(paths.size(), [&](uint32_t begin, uint32_t end) {
		RouteBatch batch (routes, end - begin);
		for (uint32_t i = begin; i < end; i++) {
			Ray ray = paths.ray(i);
			float t;
			uint32_t prim;
			if (scene.closestHit(ray, 1e-3f, kInfinity, t, prim)) {
				paths.hitT[i] = t;
				paths.hitPrim[i] = prim;
				batch.local.material[static_cast<uint32_t>(scene.materialOf(prim).type)].push(i);
			}
			else {
				paths.hitPrim[i] = kNoHit;
				batch.local.miss.push(i);
			}
		}
	});
}
inline void stageMissSky(const Scene& scene, const PathQueue& paths, const IndexQueue& missQueue, Film& film) {
	parallelFor(missQueue.size(), [&](uint32_t k) {
		uint32_t i = missQueue.index[k];
		Color c = paths.throughput(i) * scene.sky(paths.ray(i).dir);
		film.add(paths.pixel[i], c);
	});
}
inline Vec3 faceForward(const Vec3& outwardNormal, const Vec3& rayDir, bool& frontFace) {
	frontFace = dot(rayDir, outwardNormal) < 0.0f;
	return frontFace ? outwardNormal : -outwardNormal;
}
inline bool russianRoulette(int depth, Color& throughput, uint32_t& rng) {
	if (depth < 3) return true;
	float p = std::clamp(std::fmaxf(throughput.x, std::fmaxf(throughput.y, throughput.z)), 0.05f, 0.95f);
	if (rngNext(rng) >= p) return false;
	throughput /= p;
	return true;
}
inline void stageEmissive(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, Film& film) {
	parallelFor(queue.size(), [&](uint32_t k) {
		uint32_t i = queue.index[k];
		if (!paths.countEmission[i]) return;
		const Material& mat = scene.materialOf(paths.hitPrim[i]);
		Color c = paths.throughput(i) * mat.emission;
		film.add(paths.pixel[i], c);
	});
}
inline bool sampleSphereCone(const Primitive& light, const Point3& p, uint32_t& rng, Vec3& dir, float& oneMinusCosMax) {
	Vec3 toCenter = light.a - p;
	float dist2 = toCenter.lengthSquared();
	float radius2 = light.radius * light.radius;
	if (dist2 <= radius2) return false;  // el punto está dentro de la esfera emisora

	float sinMax2 = radius2 / dist2;
	float cosMax = std::sqrtf(1.0f - sinMax2);
	oneMinusCosMax = sinMax2 / (1.0f + cosMax);  // 1 - cosMax

	float cosT = 1.0f - oneMinusCosMax * rngNext(rng);
	float sinT = std::sqrtf(std::fmaxf(0.0f, 1.0f - cosT * cosT));
	float phi = 2.0f * KPI * rngNext(rng);

	Vec3 w = toCenter / std::sqrtf(dist2);
	Vec3 u, v;
	buildBasis(w, u, v);
	dir = (u * std::cosf(phi) * sinT + v * std::sinf(phi) * sinT + w * cosT).normalize();
	return true;
}
inline void stageLambertian(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, ShadowQueue& shadows, bool useNEE, int depth) {
	parallelForChunks(queue.size(), [&](uint32_t begin, uint32_t end) {
		Batch<PathQueue> out(next, end - begin);
		Batch<ShadowQueue> shadowOut(shadows, end - begin);
		for (uint32_t k = begin; k < end; k++) {
			uint32_t i = queue.index[k];
			Ray ray = paths.ray(i);
			uint32_t rng = paths.rng[i];

			Point3 p = ray.at(paths.hitT[i]);
			bool front;
			Vec3 n = faceForward(scene.outwardNormal(paths.hitPrim[i], p), ray.dir, front);
			const Material& mat = scene.materialOf(paths.hitPrim[i]);
			Color throughput = paths.throughput(i);

			const auto& lights = scene.getLights();
			if (useNEE && !lights.empty()) {
				uint32_t numLights = static_cast<uint32_t>(lights.size());
				uint32_t pick = std::min(numLights - 1, static_cast<uint32_t>(rngNext(rng) * numLights));
				const Primitive& light = scene.primitive(lights[pick]);

				Vec3 lightDir;
				float oneMinusCos;
				float cosSurface;
				float tLight;
				if (sampleSphereCone(light, p, rng, lightDir, oneMinusCos) && (cosSurface = dot(n, lightDir)) > 0.0f && intersectSphere(light.a, light.radius, {p, lightDir}, 0.0f, kInfinity, tLight)) {
					const Material& lightMat = scene.materialOf(lights[pick]);
					float weight = 2.0f * oneMinusCos * cosSurface * static_cast<float>(numLights);
					Color contrib = throughput * lightMat.emission * mat.albedo * weight;

					ShadowQueue& shadow = shadowOut.local;
					uint32_t j = shadow.append();
					shadow.ox[j] = p.x; shadow.oy[j] = p.y; shadow.oz[j] = p.z;
					shadow.dx[j] = lightDir.x; shadow.dy[j] = lightDir.y; shadow.dz[j] = lightDir.z;
					shadow.tMax[j] = tLight * 0.999f;
					shadow.cr[j] = contrib.x; shadow.cg[j] = contrib.y; shadow.cb[j] = contrib.z;
					shadow.pixel[j] = paths.pixel[i];
				}
			}
			Vec3 newDir = n + randomUnitVector(rng);
			if(newDir.nearZero()) newDir = n;

			Color newThroughput = throughput * mat.albedo;
			if (!russianRoulette(depth, newThroughput, rng)) continue;
			PathQueue& nextQueue = out.local;
			uint32_t j = nextQueue.append();
			nextQueue.setRay(j, {p, newDir.normalize()});
			nextQueue.pixel[j] = paths.pixel[i];
			nextQueue.rng[j] = rng;
			nextQueue.setThroughput(j, newThroughput);
			nextQueue.countEmission[j] = useNEE ? 0 : 1;
		}
	});
}
inline void stageShadow(const Scene& scene, const ShadowQueue& shadows, Film& film) {
	parallelFor(shadows.size(), [&](uint32_t i) {
		Ray ray{{shadows.ox[i], shadows.oy[i], shadows.oz[i]}, {shadows.dx[i], shadows.dy[i], shadows.dz[i]}};
		if (!scene.occluded(ray, 1e-3f, static_cast<float>(shadows.tMax[i]))) {
			Color contrib{shadows.cr[i], shadows.cg[i], shadows.cb[i]};
			film.add(shadows.pixel[i], contrib);
		}
	});
}
inline void stageMetal(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, int depth) {
	parallelForChunks(queue.size(), [&](uint32_t begin, uint32_t end) {
		Batch<PathQueue> out(next, end - begin);
		for (uint32_t k = begin; k < end; k++) {
			uint32_t i = queue.index[k];
			Ray ray = paths.ray(i);
			uint32_t rng = paths.rng[i];

			Point3 p = ray.at(paths.hitT[i]);
			bool front;
			Vec3 n = faceForward(scene.outwardNormal(paths.hitPrim[i], p), ray.dir, front);
			const Material& mat = scene.materialOf(paths.hitPrim[i]);

			Vec3 reflected = reflect(ray.dir, n) + mat.fuzz * randomUnitVector(rng);

			if (dot(reflected, n) <= 0.0f) continue;

			Color newThroughput = paths.throughput(i) * mat.albedo;
			if (!russianRoulette(depth, newThroughput, rng)) continue;
			PathQueue& nextQueue = out.local;
			uint32_t j = nextQueue.append();
			nextQueue.setRay(j, {p, reflected.normalize()});
			nextQueue.pixel[j] = paths.pixel[i];
			nextQueue.rng[j] = rng;
			nextQueue.setThroughput(j, newThroughput);
			nextQueue.countEmission[j] = 1;
		}
	});
}
inline float schlick(float cosine, float refIdx) {
	float r0 = (1.0f - refIdx) / (1.0f + refIdx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
}
inline void stageDielectric(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, int depth) {
	parallelForChunks(queue.size(), [&](uint32_t begin, uint32_t end) {
		Batch<PathQueue> out(next, end - begin);
		for (uint32_t k = begin; k < end; k++) {
			uint32_t i = queue.index[k];
			Ray ray = paths.ray(i);
			uint32_t rng = paths.rng[i];

			Point3 p = ray.at(paths.hitT[i]);
			bool front;
			Vec3 n = faceForward(scene.outwardNormal(paths.hitPrim[i], p), ray.dir, front);
			const Material& mat = scene.materialOf(paths.hitPrim[i]);

			float refractionRatio = front ? (1.0f / mat.ior) : mat.ior;
			Vec3 unitDir = ray.dir.normalize();
			float cosTheta = std::fmin(dot(-unitDir, n), 1.0f);
			float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

			bool cannotRefract = refractionRatio * sinTheta > 1.0f;
			bool refracts = !cannotRefract && (schlick(cosTheta, refractionRatio) < rngNext(rng));
			Vec3 direction = refracts ? refract(unitDir, n, refractionRatio) : reflect(unitDir, n);

			Color newThroughput = paths.throughput(i);
			if (refracts) newThroughput *= mat.albedo;
			if (!russianRoulette(depth, newThroughput, rng)) continue;
			PathQueue& nextQueue = out.local;
			uint32_t j = nextQueue.append();
			nextQueue.setRay(j, {p, direction});
			nextQueue.pixel[j] = paths.pixel[i];
			nextQueue.rng[j] = rng;
			nextQueue.setThroughput(j, newThroughput);
			nextQueue.countEmission[j] = 1;
		}
		
	});
}
