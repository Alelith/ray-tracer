#ifndef WAVEFRONT_HPP
# define WAVEFRONT_HPP

# include "Objects/Camera.hpp"
# include "Objects/Film.hpp"
# include "Objects/Scene.hpp"
# include "Rendering/Stages.hpp"
# include "Utils/Profiler.hpp"

struct RenderSettings {
	int			width = 800;
	int			height = 450;
	int			maxDepth = 24;
	bool		useNEE = true;
	bool		useRussianRoulette = true;
	uint32_t	maxPathsInFlight = 1u << 20;
};

class WavefrontRenderer
{
	public:
		inline WavefrontRenderer(const Scene& scene_, const Camera& camera_, const RenderSettings& settings_);
		
		inline void		renderPass(uint32_t sampleIndex, Film& film);

	private:
		inline void		renderBatch(uint32_t sampleIndex, uint32_t firstPixel, uint32_t count, Film& film);

		const Scene&	scene;
		const Camera&	camera;
		RenderSettings	settings;
		uint32_t		batchSize = 0;
		PathQueue		queues[2];
		RouteQueues		routes;
		ShadowQueue		shadows;
};

# include "detail/Rendering/Wavefront.ipp"

#endif // WAVEFRONTRENDERER_HPP
