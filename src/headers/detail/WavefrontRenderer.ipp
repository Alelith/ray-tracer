inline WavefrontRenderer::WavefrontRenderer(const Scene& scene_, const Camera& camera_, const RenderSettings& settings_) : scene(scene_), camera(camera_), settings(settings_) {
	uint32_t capacity = static_cast<uint32_t>(settings.width) * static_cast<uint32_t>(settings.height);
	batchSize = std::min(capacity, std::max(1u, settings.maxPathsInFlight));
	queues[0].allocate(capacity);
	queues[1].allocate(capacity);
	routes.allocate(capacity);
	shadows.allocate(capacity);
}

inline void WavefrontRenderer::renderPass(uint32_t sampleIndex, Film& film) {
	uint32_t capacity = static_cast<uint32_t>(settings.width) * static_cast<uint32_t>(settings.height);
	for (uint32_t firstPixel = 0; firstPixel < capacity; firstPixel += batchSize) {
		uint32_t count = std::min(batchSize, capacity - firstPixel);
		renderBatch(sampleIndex, firstPixel, count, film);
	}
}

inline void WavefrontRenderer::renderBatch(uint32_t sampleIndex, uint32_t firstPixel, uint32_t count, Film& film) {
	PathQueue* cur = &queues[0];
	PathQueue* next = &queues[1];

	{
		ScopedStageTimer timer(StageID::Generate);
		stageGenerateCamera(camera, settings.width, settings.height, sampleIndex, firstPixel, count, *cur);
	}

	for (int depth = 0; depth < settings.maxDepth && cur->size() > 0; ++depth) {
		{
			ScopedStageTimer timer(StageID::Intersect);
			stageIntersect(scene, *cur, routes);
		}
		{
			ScopedStageTimer timer(StageID::Miss);
			stageMissSky(scene, *cur, routes.miss, film);
		}
		{
			ScopedStageTimer timer(StageID::Emissive);
			stageEmissive(scene, *cur, routes.material[static_cast<uint32_t>(MaterialType::Emissive)], film);
		}

		int rrDepth = settings.useRussianRoulette ? depth : 0;

		next->clear();
		shadows.clear();
		{
			ScopedStageTimer timer(StageID::Lambertian);
			stageLambertian(scene, *cur, routes.material[static_cast<uint32_t>(MaterialType::Lambertian)], *next, shadows, settings.useNEE, rrDepth);
		}
		{
			ScopedStageTimer timer(StageID::Metal);
			stageMetal(scene, *cur, routes.material[static_cast<uint32_t>(MaterialType::Metal)], *next, rrDepth);
		}
		{
			ScopedStageTimer timer(StageID::Dielectric);
			stageDielectric(scene, *cur, routes.material[static_cast<uint32_t>(MaterialType::Dielectric)], *next, rrDepth);
		}
		{
			ScopedStageTimer timer(StageID::Intersect);
			stageShadow(scene, shadows, film);
		}
		std::swap(cur, next);
	}
}
