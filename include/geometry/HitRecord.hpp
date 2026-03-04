#ifndef HITRECORD_HPP
# define HITRECORD_HPP

# include "Ray.hpp"

struct HitRecord
{
	Point3	point;
	Vector3	normal;
	double	t;
	bool	frontFace;
	bool	hit;

	HitRecord() : t(INFINITY), frontFace(false), hit(false) {}
	HitRecord(bool hit, const Point3 &point, const Vector3 &normal, double t, bool frontFace)
		: point(point), normal(normal), t(t), frontFace(frontFace), hit(hit) {}

	inline void	setFaceNormal(const Ray &ray, const Vector3 &outwardNormal)
	{
		frontFace = dot(ray.getDirection(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

#endif /* HITRECORD_HPP */
