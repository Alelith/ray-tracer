#ifndef MEGAKERNEL_HPP
# define MEGAKERNEL_HPP

# include "Objects/Camera.hpp"
# include "Objects/Film.hpp"
# include "Utils/Parallel.hpp"
# include "Objects/Scene.hpp"
# include "Rendering/Stages.hpp"
# include "Rendering/Wavefront.hpp"

inline Color tracePathMegakernel(const Scene& scene, Ray ray, uint32_t& rng, int maxDepth, bool useNEE, bool useRR);
inline void renderMegakernelPass(const Scene& scene, const Camera& camera, const RenderSettings& settings, uint32_t sampleIndex, Film& film);

# include "detail/Rendering/Megakernel.ipp"

# endif // MEGAKERNEL_HPP
