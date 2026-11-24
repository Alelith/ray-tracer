#ifndef MEGAKERNEL_HPP
# define MEGAKERNEL_HPP

# include "Camera.hpp"
# include "Film.hpp"
# include "Parallel.hpp"
# include "Scene.hpp"
# include "Stages.hpp"

inline Color tracePathMegakernel(const Scene& scene, Ray ray, uint32_t& rng, int maxDepth, bool useNEE);
inline void renderMegakernelPass(const Scene& scene, const Camera& camera, int width, int height, int maxDepth, bool useNEE, uint32_t sampleIndex, Film& film);

# include "detail/Megakernel.ipp"

# endif // MEGAKERNEL_HPP
