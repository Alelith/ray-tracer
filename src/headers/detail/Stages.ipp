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
inline Color skyColor(const Vec3& dir) {
	// Real sky color
	float t = 0.5f * (dir.y + 1.0f);
	return lerp(Color(1.0f, 1.0f, 1.0f), Color(0.5f, 0.7f, 1.0f), t);
	// Direction test
	/*return 0.5f * (dir + Vec3(1.0f, 1.0f, 1.0f));*/
}
inline void stageMissSky(const PathQueue& paths, const IndexQueue& missQueue, Film& film) {
	parallelFor(missQueue.size(), [&](uint32_t k) {
		uint32_t i = missQueue.index[k];
		Color c = paths.throughput(i) * skyColor(paths.ray(i).dir);
		film.add(paths.pixel[i], c);
	});
}
inline Vec3 faceForward(const Vec3& outwardNormal, const Vec3& rayDir, bool& frontFace) {
	frontFace = dot(rayDir, outwardNormal) < 0.0f;
	return frontFace ? outwardNormal : -outwardNormal;
}
inline void stageLambertian(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next) {
	parallelFor(queue.size(), [&](uint32_t k) {
		uint32_t i = queue.index[k];
		Ray ray = paths.ray(i);
		uint32_t rng = paths.rng[i];

		Point3 p = ray.at(paths.hitT[i]);
		bool front;
		Vec3 n = faceForward(scene.outwardNormal(paths.hitPrim[i], p), ray.dir, front);
		const Material& mat = scene.materialOf(paths.hitPrim[i]);

		Vec3 dir = n + randomUnitVector(rng);
		if (dir.nearZero()) dir = n;

		uint32_t j = next.append();
		next.setRay(j, {p, dir.normalize()});
		next.pixel[j] = paths.pixel[i];
		next.rng[j] = rng;
		next.setThroughput(j, paths.throughput(i) * mat.albedo);
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
		Vec3 dir;
		if (cannotRefract || schlick(cosTheta, eta) > rngNext(rng))
			dir = reflect(unitDir, n);
		else
			dir = refract(unitDir, n, eta);

		uint32_t j = next.append();
		next.setRay(j, {p, dir.normalize()});
		next.pixel[j] = paths.pixel[i];
		next.rng[j] = rng;
		next.setThroughput(j, paths.throughput(i));
	});
}
