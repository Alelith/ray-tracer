#ifndef SCENE_HPP
# define SCENE_HPP

# include <cstdint>
# include <limits>
# include <vector>
# include "Ray.hpp"

constexpr uint32_t	kNoHit = std::numeric_limits<uint32_t>::max();
constexpr float		kInfinity = std::numeric_limits<float>::infinity();

struct Sphere {
	Point3	center;
	float	radius;
};

inline bool		intersectSphere(const Sphere& sphere, const Ray& ray, float tMin, float tMax, float& tHit);

class Scene {
	public:
		inline uint32_t		addSphere(Point3 center, float radius);
		inline bool			closestHit(const Ray& ray, float tMin, float tMax, float& tHit, uint32_t& primHit) const;
		inline Vec3			outwardNormal(uint32_t prim, const Point3& p) const;
	
	private:
		std::vector<Sphere>	spheres;
};

# include "detail/Scene.ipp"

#endif // SCENE_HPP
