#ifndef RAY_HPP
# define RAY_HPP

# include "Maths/Vec3.hpp"

struct Ray {
	Point3	origin;
	Vec3	dir;

	inline Point3	at(float t) const;
};

# include "detail/Maths/Ray.ipp"

#endif // RAY_HPP
