#ifndef RAY_HPP
# define RAY_HPP

# include "Vector3.hpp"
# include "Point3.hpp"

class Ray
{
	private:
		Point3	origin;
		Vector3	direction;
	public:
		Ray();
		Ray(const Point3 &origin, const Vector3 &direction);
		Ray(const Ray &other) = default;
		~Ray() = default;

		inline Point3	getOrigin() const { return origin; }
		inline Vector3	getDirection() const { return direction; }

		inline Point3	at(float t) const { return origin + direction * t; }
};

std::ostream	&operator<<(std::ostream &os, const Ray &ray);

#endif /* RAY_HPP */
