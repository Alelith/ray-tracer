#ifndef STAGES_HPP
# define STAGES_HPP

# include "Camera.hpp"
# include "PathQueue.hpp"
# include "Film.hpp"
# include "Scene.hpp"

inline void		stageGenerateCamera(const Camera& cam, int width, int height, PathQueue& out);
inline void		stageIntersect(const Scene& scene, PathQueue& paths, IndexQueue& missQueue, IndexQueue& hitQueue);
inline Color	skyColor(const Vec3& dir);
inline void		stageMissSky(const PathQueue& in, const IndexQueue& missQueue, Film& film);
inline void		stageShadeNormals(const Scene& scene, const PathQueue& paths, const IndexQueue& hitQueue, Film& film);

# include "detail/Stages.ipp"

#endif // STAGES_HPP
