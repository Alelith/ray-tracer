#ifndef HITRECORD_HPP
# define HITRECORD_HPP

# include "Ray.cuh"

struct HitRecord
{
	Point3	point;
	Vector3	normal;
	float	t;
	int		geomIdx;  // index into BVHView::geoms — filled by bvhHit
	bool	frontFace;
	bool	hit;

	float	u;  // texture coordinates
	float	v;  // texture coordinates

	RT_CALLABLE HitRecord() : t(INFINITY), geomIdx(-1), frontFace(false), hit(false), u(0), v(0) {}
	RT_CALLABLE HitRecord(bool hit, const Point3 &point, const Vector3 &normal, float t, bool frontFace)
		: point(point), normal(normal), t(t), geomIdx(-1), frontFace(frontFace), hit(hit), u(0), v(0) {}

	inline RT_CALLABLE void	setFaceNormal(const Ray &ray, const Vector3 &outwardNormal)
	{
		frontFace = dot(ray.getDirection(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

#endif /* HITRECORD_HPP */
