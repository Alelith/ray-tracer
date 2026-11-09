#ifndef CAMERA_HPP
# define CAMERA_HPP

# include <cstdint>
# include "Ray.hpp"

class Camera {
	public:
		inline Camera(Point3 lookFrom, Point3 lookAt, Vec3 up, float verticalFovDeg, float aspectRatio, float aperture = 0.0f, float focusDist = 1.0f);
		
		inline Ray	getRay(float s, float t, uint32_t& rng) const;

	private:
		Point3	origin;
		Point3	lowerLeft;
		Vec3	horizontal;
		Vec3	vertical;
		Vec3	u, v;
		float	lensRadius = 0.0f;
};

# include "detail/Camera.ipp"

#endif // CAMERA_HPP
