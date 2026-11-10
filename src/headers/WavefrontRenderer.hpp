#ifndef WAVEFRONTRENDERER_HPP
# define WAVEFRONTRENDERER_HPP

# include "Camera.hpp"
# include "Film.hpp"
# include "Scene.hpp"
# include "Stages.hpp"

class WavefrontRenderer
{
	public:
		inline WavefrontRenderer(const Scene& scene_, const Camera& camera_, int width_, int height_, int maxDepth_);
		
		inline void		renderPass(uint32_t sampleIndex, Film& film, bool printStats = false);

	private:
		const Scene&	scene;
		const Camera&	camera;
		int				width, height, maxDepth;
		PathQueue		queues[2];
		RouteQueues		routes;
};

# include "detail/WavefrontRenderer.ipp"

#endif // WAVEFRONTRENDERER_HPP
