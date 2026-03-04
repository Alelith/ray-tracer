#ifndef RAY_HPP
# define RAY_HPP

# include "Vector3.hpp"

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

		Point3	getOrigin() const;
		Vector3	getDirection() const;

		Point3	at(double t) const;
};

std::ostream	&operator<<(std::ostream &os, const Ray &ray);

#endif /* RAY_HPP */
