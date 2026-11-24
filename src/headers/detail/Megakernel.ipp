inline Color tracePathMegakernel(const Scene& scene, Ray ray, uint32_t& rng, int maxDepth, bool useNEE) {
	Color radiance(0, 0, 0);
	Color throughput(1, 1, 1);
	bool countEmission = true;

	for (int depth = 0; depth < maxDepth; ++depth) {
		float t;
		uint32_t prim;
		if (!scene.closestHit(ray, 1e-3f, kInfinity, t, prim)) {
			radiance += throughput * scene.sky(ray.dir);
			break;
		}
		const Material& mat = scene.materialOf(prim);
		if (mat.type == MaterialType::Emissive) {
			if (countEmission) radiance += throughput * mat.emission;
			break;
		}

		Point3 p = ray.at(t);
		bool front;
		Vec3 n = faceForward(scene.outwardNormal(prim, p), ray.dir, front);

		if (mat.type == MaterialType::Lambertian) {
			const auto& lights = scene.getLights();
			if (useNEE && !lights.empty()) {
				uint32_t numLights = uint32_t(lights.size());
				uint32_t pick = std::min(numLights - 1, uint32_t(rngNext(rng) * float(numLights)));
				const Primitive& light = scene.primitive(lights[pick]);
				Vec3 ldir;
				float oneMinusCos, cosSurface, tLight;
				if (sampleSphereCone(light, p, rng, ldir, oneMinusCos) && (cosSurface = dot(n, ldir)) > 0.0f &&
					intersectSphere(light.a, light.radius, {p, ldir}, 0.0f, kInfinity, tLight) &&
					!scene.occluded({p, ldir}, 1e-3f, tLight * 0.999f)) {
					float weight = 2.0f * oneMinusCos * cosSurface * float(numLights);
					radiance += throughput * mat.albedo * scene.materialOf(lights[pick]).emission * weight;
				}
			}
			Vec3 dir = n + randomUnitVector(rng);
			if (dir.nearZero()) dir = n;
			ray = {p, dir.normalize()};
			throughput *= mat.albedo;
			countEmission = !useNEE;
		} else if (mat.type == MaterialType::Metal) {
			Vec3 dir = reflect(ray.dir, n) + mat.fuzz * randomUnitVector(rng);
			if (dot(dir, n) <= 0.0f) break;
			ray = {p, dir.normalize()};
			throughput *= mat.albedo;
			countEmission = true;
		} else {  // Dielectric
			float eta = front ? 1.0f / mat.ior : mat.ior;
			Vec3 unitDir = ray.dir.normalize();
			float cosTheta = std::fmin(dot(-unitDir, n), 1.0f);
			float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
			bool cannotRefract = eta * sinTheta > 1.0f;
			bool refracts = !cannotRefract && schlick(cosTheta, eta) <= rngNext(rng);
			Vec3 dir = refracts ? refract(unitDir, n, eta) : reflect(unitDir, n);
			ray = {p, dir.normalize()};
			if (refracts) throughput *= mat.albedo;
			countEmission = true;
		}
	}
	return radiance;
}

inline void renderMegakernelPass(const Scene& scene, const Camera& camera, int width, int height, int maxDepth, bool useNEE, uint32_t sampleIndex, Film& film) {
	parallelFor(uint32_t(width * height), [&](uint32_t pixelIndex) {
		int x = int(pixelIndex) % width;
		int y = int(pixelIndex) / width;
		uint32_t rng = makeSeed(pixelIndex, sampleIndex);
		float s = (float(x) + rngNext(rng)) / float(width);
		float t = 1.0f - (float(y) + rngNext(rng)) / float(height);
		Ray ray = camera.getRay(s, t, rng);
		film.add(pixelIndex, tracePathMegakernel(scene, ray, rng, maxDepth, useNEE));
	});
}
