#include "Sampling.hpp"

inline Camera::Camera(Point3 lookFrom, Point3 lookAt, Vec3 up, float verticalFovDeg, float aspectRatio, float aperture, float focusDist) {
	float theta = verticalFovDeg * KPI / 180.0f;
	float h = std::tanf(theta / 2.0f);
	float viewportHeight = 2.0f * h;
	float viewportWidth = aspectRatio * viewportHeight;

	Vec3 w = (lookFrom - lookAt).normalize();
	Vec3 u = cross(up, w).normalize();
	Vec3 v = cross(w, u);

	origin = lookFrom;
	horizontal = focusDist * viewportWidth * u;
	vertical = focusDist * viewportHeight * v;
	lowerLeft = origin - horizontal / 2.0f - vertical / 2.0f - focusDist * w;
	lensRadius = aperture / 2.0f;
	this->u = u;
	this->v = v;
}

inline Ray Camera::getRay(float s, float t, uint32_t& rng) const {
	Vec3 offset(0, 0, 0);
	if (lensRadius > 0.0f) {
		Vec3 rd = lensRadius * randomInUnitDisk(rng);
		offset = u * rd.x + v * rd.y;
	}
	Point3 o = origin + offset;
	Vec3 dir = lowerLeft + s * horizontal + t * vertical - o;
	return {o, dir.normalize()};
}
