#ifndef RAY_HPP
# define RAY_HPP

# include "Point3.cuh"

class Ray
{
	private:
		Point3	origin;
		Vector3	direction;
	public:
		RT_CALLABLE Ray();
		RT_CALLABLE Ray(const Point3 &origin, const Vector3 &direction);
		RT_CALLABLE Ray(const Ray &other) = default;
		RT_CALLABLE ~Ray() = default;

		inline RT_CALLABLE Point3	getOrigin() const { return origin; }
		inline RT_CALLABLE Vector3	getDirection() const { return direction; }
		inline RT_CALLABLE Point3	at(float t) const { return origin + direction * t; }
};
std::ostream	&operator<<(std::ostream &os, const Ray &ray);
#endif /* RAY_HPP */
