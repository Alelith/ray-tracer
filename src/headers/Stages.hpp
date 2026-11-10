#ifndef STAGES_HPP
# define STAGES_HPP

# include <array>
# include "Camera.hpp"
# include "PathQueue.hpp"
# include "Film.hpp"
# include "Scene.hpp"

struct RouteQueues {
	IndexQueue								miss;
	std::array<IndexQueue, kMaterialCount>	material;

	inline void		allocate(uint32_t capacity);
	inline void		clear();
};

inline void		stageGenerateCamera(const Camera& cam, int width, int height, uint32_t sampleIndex, PathQueue& out);
inline void		stageIntersect(const Scene& scene, PathQueue& paths, RouteQueues& routes);
inline Color	skyColor(const Vec3& dir);
inline void		stageMissSky(const PathQueue& in, const IndexQueue& missQueue, Film& film);
inline Vec3		faceForward(const Vec3& outwardNormal, const Vec3& rayDir, bool& frontFace);
inline void		stageLambertian(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next);
inline void		stageMetal(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next);
inline float	schlick(float cosine, float refIdx);
inline void		stageDielectric(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next);

# include "detail/Stages.ipp"

#endif // STAGES_HPP
