inline Camera::Camera(Point3 lookFrom, Point3 lookAt, Vec3 up, float verticalFovDeg, float aspectRatio) {
	float theta = verticalFovDeg * KPI / 180.0f;
	float h = std::tanf(theta / 2.0f);
	float viewportHeight = 2.0f * h;
	float viewportWidth = aspectRatio * viewportHeight;

	Vec3 w = (lookFrom - lookAt).normalize();
	Vec3 u = cross(up, w).normalize();
	Vec3 v = cross(w, u);

	origin = lookFrom;
	horizontal = viewportWidth * u;
	vertical = viewportHeight * v;
	lowerLeft = origin - horizontal / 2.0f - vertical / 2.0f - w;
}
		
inline Ray Camera::getRay(float s, float t) {
	Vec3 dir = lowerLeft + s * horizontal + t * vertical - origin;
	return {origin, dir.normalize()};
}
