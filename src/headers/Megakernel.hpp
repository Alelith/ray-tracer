#ifndef MEGAKERNEL_HPP
# define MEGAKERNEL_HPP

# include "Camera.hpp"
# include "Film.hpp"
# include "Parallel.hpp"
# include "Scene.hpp"
# include "Stages.hpp"
# include "WavefrontRenderer.hpp"

inline Color tracePathMegakernel(const Scene& scene, Ray ray, uint32_t& rng, int maxDepth, bool useNEE, bool useRR);
inline void renderMegakernelPass(const Scene& scene, const Camera& camera, const RenderSettings& settings, uint32_t sampleIndex, Film& film);

# include "detail/Megakernel.ipp"

# endif // MEGAKERNEL_HPP
