inline WavefrontRenderer::WavefrontRenderer(const Scene& scene_, const Camera& camera_, int width_, int height_, int maxDepth_) : scene(scene_), camera(camera_), width(width_), height(height_), maxDepth(maxDepth_) {
	uint32_t capacity = static_cast<uint32_t>(width) * static_cast<uint32_t>(height);
	queues[0].allocate(capacity);
	queues[1].allocate(capacity);
	routes.allocate(capacity);
}

inline void WavefrontRenderer::renderPass(uint32_t sampleIndex, Film& film, bool printStats) {
	PathQueue* cur = &queues[0];
	PathQueue* next = &queues[1];

	stageGenerateCamera(camera, width, height, sampleIndex, *cur);

	for (int depth = 0; depth < maxDepth && cur->size() > 0 ; depth++) {
		if (printStats) std::printf("  rebote %2d: %6u activos | fallos %6u | difuso %6u | metal %6u | vidrio %6u\n", depth,
									cur->size(), routes.miss.size(), routes.material[0].size(),
									routes.material[1].size(), routes.material[2].size());

		stageIntersect(scene, *cur, routes);
		stageMissSky(*cur, routes.miss, film);

		next->clear();
		stageLambertian(scene, *cur, routes.material[static_cast<uint32_t>(MaterialType::Lambertian)], *next);
		stageMetal(scene, *cur, routes.material[uint32_t(MaterialType::Metal)], *next);
		stageDielectric(scene, *cur, routes.material[uint32_t(MaterialType::Dielectric)], *next);

		std::swap(cur, next);
	}
}