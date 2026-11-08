#ifndef STAGES_HPP
# define STAGES_HPP

# include "Camera.hpp"
# include "PathQueue.hpp"
# include "Film.hpp"

inline void		stageGenerateCamera(const Camera& cam, int width, int height, PathQueue& out);
inline Color	skyColor(const Vec3& dir);
inline void		stageMissSky(const PathQueue& in, Film& film);

# include "detail/Stages.ipp"

#endif // STAGES_HPP
