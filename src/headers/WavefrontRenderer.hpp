#ifndef WAVEFRONTRENDERER_HPP
# define WAVEFRONTRENDERER_HPP

# include "Camera.hpp"
# include "Film.hpp"
# include "Scene.hpp"
# include "Stages.hpp"

class WavefrontRenderer
{
	public:
		inline WavefrontRenderer(const Scene& scene_, const Camera& camera_, int width_, int height_, int maxDepth_, bool useNEE_ = true);
		
		inline void		renderPass(uint32_t sampleIndex, Film& film, bool printStats = false);

	private:
		const Scene&	scene;
		const Camera&	camera;
		int				width, height, maxDepth;
		bool			useNEE;
		PathQueue		queues[2];
		RouteQueues		routes;
		ShadowQueue		shadows;
};

# include "detail/WavefrontRenderer.ipp"

#endif // WAVEFRONTRENDERER_HPP
