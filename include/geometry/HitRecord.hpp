#ifndef HITRECORD_HPP
# define HITRECORD_HPP

# include "Ray.hpp"

struct HitRecord
{
	bool	hit;
	Point3	point;
	Vector3	normal;
	double	t;
	bool	frontFace;

	HitRecord();
	HitRecord(bool hit, const Point3 &point, const Vector3 &normal, double t, bool frontFace);
	inline void	setFaceNormal(const Ray &ray, const Vector3 &outwardNormal)
	{
		frontFace = dot(ray.getDirection(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

#endif /* HITRECORD_HPP */
