#ifndef HITRECORD_HPP
# define HITRECORD_HPP

# include "Ray.hpp"
# include <memory>

using std::shared_ptr;

class Material;

class HitRecord
{
	public:
		Point3					point;
		Vector3					normal;
		shared_ptr<Material>	material;
		double					t;
		bool					frontFace;

		inline void setFaceNormal(const Ray &ray, const Vector3 &outwardNormal)
		{
			frontFace = dot(ray.getDirection(), outwardNormal) < 0;
			normal = frontFace ? outwardNormal : -outwardNormal;
		}
};

#endif /* HITRECORD_HPP */
