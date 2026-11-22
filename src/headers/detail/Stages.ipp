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

inline void stageGenerateCamera(const Camera& cam, int width, int height, uint32_t sampleIndex, PathQueue& out) {
	out.clear();
	parallelFor(static_cast<uint32_t>(width * height), [&](uint32_t pixelIndex) {
		int x = static_cast<int>(pixelIndex) % width;
		int y = static_cast<int>(pixelIndex) / width;
		uint32_t rng = makeSeed(pixelIndex, sampleIndex);

		float s = (static_cast<float>(x) + rngNext(rng)) / static_cast<float>(width);
		float t = 1.0f - (static_cast<float>(y) + rngNext(rng)) / static_cast<float>(height);

		uint32_t slot = out.append();
		out.setRay(slot, cam.getRay(s, t, rng));
		out.pixel[slot] = pixelIndex;
		out.rng[slot] = rng;
		out.setThroughput(slot, Color(1, 1, 1));
		out.countEmission[slot] = 1;
	});
}
inline void stageIntersect(const Scene& scene, PathQueue& paths, RouteQueues& routes) {
	routes.clear();
	parallelFor(paths.size(), [&](uint32_t i) {
		Ray ray = paths.ray(i);
		float t;
		uint32_t prim;
		//if (scene.closestHit(ray, 0.0f, kInfinity, t, prim)) {
		if (scene.closestHit(ray, 1e-4f, kInfinity, t, prim)) {
			paths.hitT[i] = t;
			paths.hitPrim[i] = prim;
			routes.material[static_cast<uint32_t>(scene.materialOf(prim).type)].push(i);
		}
		else {
			paths.hitPrim[i] = kNoHit;
			routes.miss.push(i);
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
inline void stageLambertian(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, ShadowQueue& shadows, bool useNEE) {
	parallelFor(queue.size(), [&](uint32_t k) {
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
				Color contrib = throughput * mat.albedo * lightMat.emission * weight;

				uint32_t s = shadows.append();
				shadows.ox[s] = p.x;
				shadows.oy[s] = p.y;
				shadows.oz[s] = p.z;
				shadows.dx[s] = lightDir.x;
				shadows.dy[s] = lightDir.y;
				shadows.dz[s] = lightDir.z;
				shadows.tmax[s] = tLight * 0.9999f;
				shadows.cr[s] = contrib.x;
				shadows.cg[s] = contrib.y;
				shadows.cb[s] = contrib.z;
				shadows.pixel[s] = paths.pixel[i];
			}
		}

		Vec3 dir = n + randomUnitVector(rng);
		if (dir.nearZero()) dir = n;

		uint32_t j = next.append();
		next.setRay(j, {p, dir.normalize()});
		next.pixel[j] = paths.pixel[i];
		next.rng[j] = rng;
		next.setThroughput(j, throughput * mat.albedo);
		next.countEmission[j] = useNEE ? 0 : 1;
	});
}
inline void stageShadow(const Scene& scene, const ShadowQueue& shadows, Film& film) {
	parallelFor(shadows.size(), [&](uint32_t i) {
		Ray ray{{shadows.ox[i], shadows.oy[i], shadows.oz[i]}, {shadows.dx[i], shadows.dy[i], shadows.dz[i]}};
		if (!scene.occluded(ray, 1e-3f, static_cast<float>(shadows.tmax[i]))) {
			Color contrib{shadows.cr[i], shadows.cg[i], shadows.cb[i]};
			film.add(shadows.pixel[i], contrib);
		}
	});
}
inline void stageMetal(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next) {
	parallelFor(queue.size(), [&](uint32_t k) {
		uint32_t i = queue.index[k];
		Ray ray = paths.ray(i);
		uint32_t rng = paths.rng[i];

		Point3 p = ray.at(paths.hitT[i]);
		bool front;
		Vec3 n = faceForward(scene.outwardNormal(paths.hitPrim[i], p), ray.dir, front);
		const Material& mat = scene.materialOf(paths.hitPrim[i]);

		Vec3 dir = reflect(ray.dir, n) +  mat.fuzz * randomUnitVector(rng);
		// Si la perturbación manda el rayo bajo la superficie, el camino se absorbe.
		if (dot(dir, n) <= 0.0f) return;

		uint32_t j = next.append();
		next.setRay(j, {p, dir.normalize()});
		next.pixel[j] = paths.pixel[i];
		next.rng[j] = rng;
		next.setThroughput(j, paths.throughput(i) * mat.albedo);
		next.countEmission[j] = 1;
	});
}
inline float schlick(float cosine, float refIdx) {
	float r0 = (1.0f - refIdx) / (1.0f + refIdx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
}
inline void stageDielectric(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next) {
	parallelFor(queue.size(), [&](uint32_t k) {
		uint32_t i = queue.index[k];
		Ray ray = paths.ray(i);
		uint32_t rng = paths.rng[i];

		Point3 p = ray.at(paths.hitT[i]);
		bool front;
		Vec3 n = faceForward(scene.outwardNormal(paths.hitPrim[i], p), ray.dir, front);
		const Material& mat = scene.materialOf(paths.hitPrim[i]);

		float eta = front ? 1.0f / mat.ior : mat.ior;
		Vec3 unitDir = ray.dir.normalize();
		float cosTheta = std::fmin(dot(-unitDir, n), 1.0f);
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		bool cannotRefract = eta * sinTheta > 1.0f;  // reflexión total interna
		bool refracts = !cannotRefract && schlick(cosTheta, eta) <= rngNext(rng);
		Vec3 dir = refracts ? refract(unitDir, n, eta) : reflect(unitDir, n);
		Color survived = paths.throughput(i);
		if (refracts) survived *= mat.albedo;

		uint32_t j = next.append();
		next.setRay(j, {p, dir.normalize()});
		next.pixel[j] = paths.pixel[i];
		next.rng[j] = rng;
		next.setThroughput(j, survived);
		next.countEmission[j] = 1;
	});
}
