#include "Parallel.hpp"
#include "RNG.hpp"

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
	});
}
inline void stageIntersect(const Scene& scene, PathQueue& paths, IndexQueue& missQueue, IndexQueue& hitQueue) {
	missQueue.clear();
	hitQueue.clear();
	parallelFor(paths.size(), [&](uint32_t i) {
		Ray ray = paths.ray(i);
		float t;
		uint32_t prim;
		if (scene.closestHit(ray, 0.0f, kInfinity, t, prim)) {
		//if (scene.closestHit(ray, EPSILON, kInfinity, t, prim)) {
			paths.hitT[i] = t;
			paths.hitPrim[i] = prim;
			hitQueue.push(i);
		}
		else {
			paths.hitPrim[i] = kNoHit;
			missQueue.push(i);
		}
	});
}
inline Color skyColor(const Vec3& dir) {
	// Real sky color
	float t = 0.5f * (dir.y + 1.0f);
	return lerp(Color(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f), t);
	// Direction test
	/*return 0.5f * (dir + Vec3(1.0f, 1.0f, 1.0f));*/
}
inline void stageMissSky(const PathQueue& paths, const IndexQueue& missQueue, Film& film) {
	parallelFor(missQueue.size(), [&](uint32_t k) {
		uint32_t i = missQueue.index[k];
		film.add(paths.pixel[i], skyColor(paths.ray(i).dir));
	});
}
inline void stageShadeNormals(const Scene& scene, const PathQueue& paths, const IndexQueue& hitQueue, Film& film) {
	parallelFor(hitQueue.size(), [&](uint32_t k) {
		uint32_t i = hitQueue.index[k];
		Ray ray = paths.ray(i);
		Point3 p = ray.at(paths.hitT[i]);
		Vec3 n = scene.outwardNormal(paths.hitPrim[i], p);
		film.add(paths.pixel[i], 0.5f * (n + Vec3(1, 1, 1)));
	});
}
