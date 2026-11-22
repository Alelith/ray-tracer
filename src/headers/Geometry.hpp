#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include <limits>
# include <cstdint>
# include "Ray.hpp"

constexpr uint32_t	kNoHit = std::numeric_limits<uint32_t>::max();
constexpr float		kInfinity = std::numeric_limits<float>::infinity();

enum class PrimType : uint32_t { Sphere, Triangle };

struct Primitive {
	PrimType	type = PrimType::Sphere;
	uint32_t	material = 0;
	Point3		a, b, c;
	float		radius = 0.0f;
};

inline bool		intersectSphere(const Point3& center, float radius, const Ray& ray, float tMin, float tMax, float& tHit);
inline bool		intersectTriangle(const Point3& v0, const Point3& v1, const Point3& v2, const Ray& ray, float tMin, float tMax, float& tHit);
inline bool		intersectPrimitive(const Primitive& p, const Ray& ray, float tMin, float tMax, float& tHit);

struct AABB {
	Vec3	min{kInfinity, kInfinity, kInfinity};
	Vec3	max{-kInfinity, -kInfinity, -kInfinity};

	inline void		grow(const Vec3& p);
	inline void		grow(const AABB& b);
	inline Vec3		centroid() const;
	inline Vec3		extent() const;
};

inline AABB		boundsOf(const Primitive& p);

# include "detail/Geometry.ipp"

#endif // GEOMETRY_HPP
