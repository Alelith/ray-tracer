#ifndef STAGES_HPP
# define STAGES_HPP

# include <array>
# include "Objects/Camera.hpp"
# include "Utils/PathQueue.hpp"
# include "Objects/Film.hpp"
# include "Objects/Scene.hpp"

struct RouteQueues {
	IndexQueue								miss;
	std::array<IndexQueue, kMaterialCount>	material;

	inline void		allocate(uint32_t capacity);
	inline void		clear();
};

struct RouteBatch {
	RouteBatch(RouteQueues& t, uint32_t maxEntries);
	~RouteBatch();
	RouteBatch(const RouteBatch&) = delete;
	RouteBatch&		operator=(const RouteBatch&) = delete;

	RouteQueues&	target;
	RouteQueues		local;
};

inline void		stageGenerateCamera(const Camera& cam, int width, int height, uint32_t sampleIndex, uint32_t firstPixel, uint32_t pixelCount, PathQueue& out);
inline void		stageIntersect(const Scene& scene, PathQueue& paths, RouteQueues& routes);
inline void		stageMissSky(const Scene& scene, const PathQueue& in, const IndexQueue& missQueue, Film& film);
inline Vec3		faceForward(const Vec3& outwardNormal, const Vec3& rayDir, bool& frontFace);
inline bool		russianRoulette(int depth, Color& throughput, uint32_t& rng);
inline void		stageEmissive(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, Film& film);
inline bool		sampleSphereCone(const Primitive& light, const Point3& p, uint32_t& rng, Vec3& dir, float& oneMinusCosMax);
inline void		stageLambertian(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, ShadowQueue& shadows, bool useNEE, int depth);
inline void		stageShadow(const Scene& scene, const ShadowQueue& shadows, Film& film);
inline void		stageMetal(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, int depth);
inline float	schlick(float cosine, float refIdx);
inline void		stageDielectric(const Scene& scene, const PathQueue& paths, const IndexQueue& queue, PathQueue& next, int depth);

# include "detail/Rendering/Stages.ipp"

#endif // STAGES_HPP
