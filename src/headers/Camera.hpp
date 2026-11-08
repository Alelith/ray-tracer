#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Ray.hpp"

class Camera {
	public:
		inline Camera(Point3 lookFrom, Point3 lookAt, Vec3 up, float verticalFovDeg, float aspectRatio);
		
		inline Ray	getRay(float s, float t) const;

	private:
		Point3	origin;
		Point3	lowerLeft;
		Vec3	horizontal;
		Vec3	vertical;
};

# include "detail/Camera.ipp"

#endif // CAMERA_HPP
