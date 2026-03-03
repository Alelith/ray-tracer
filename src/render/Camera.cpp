#include "Camera.hpp"

Camera::Camera() : aspectRatio(1.0), pixelSamplesScale(1.0), vfov(90.0), defocusAngle(0.0), focalDistance(1.0), width(100), height(100), samplesPerPixel(1), depth(0), maxDepth(5), origin(Point3(0, 0, 0)), pixel00Loc(Point3(-1, 1, -1)), pixelDeltaU(Vector3(2.0 / width, 0, 0)), pixelDeltaV(Vector3(0, -2.0 / height, 0)), defocusDiskU(Vector3(1, 0, 0)), defocusDiskV(Vector3(0, 1, 0)), backgroundColor(Color(0, 0, 0))
{
	initCamera();
}

Camera::Camera(double aspectRatio, int width, int samplesPerPixel, int maxDepth, double vfov, Point3 origin, Vector3 rotation, double defocusAngle, double focalDistance, Color backgroundColor) : aspectRatio(aspectRatio), pixelSamplesScale(1.0 / samplesPerPixel), vfov(vfov), defocusAngle(defocusAngle), focalDistance(focalDistance), width(width), height(static_cast<int>(width / aspectRatio)), samplesPerPixel(samplesPerPixel), depth(maxDepth), maxDepth(maxDepth), origin(origin), backgroundColor(backgroundColor)
{
	initCamera();
}

Camera::~Camera() {}

void	Camera::initCamera(Vector3 rotation)
{
	double	theta = vfov * (M_PI / 180.0) / 2.0;
	double	halfHeight = tan(theta / 2);

	double	viewportWidth = 2 * halfHeight * focalDistance;
	double	viewportHeight = viewportWidth / aspectRatio;

	Vector3	w = -rotation.normalized();
	Vector3	u = cross(Vector3::Up, w);
	if (u.length_sqrd() < 1e-10)
		u = cross(Vector3::Right, w);
	u = u.normalized();
	Vector3	v = cross(w, u);

	pixelDeltaU = (viewportWidth / width) * u;
	pixelDeltaV = (viewportHeight / height) * v;

	Point3	viewportLowerLeft = origin - (viewportWidth / 2) * u - (viewportHeight / 2) * v - focalDistance * w;

	pixel00Loc = viewportLowerLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

	double defocusRadius = focalDistance * tan(defocusAngle / 2.0 * (M_PI / 180.0));

	defocusDiskU = defocusRadius * u;
	defocusDiskV = defocusRadius * v;
}

vector<vector<Color>> Camera::render(RenderHitList &world)
{
	vector<vector<Color>>	image(height, vector<Color>(width, backgroundColor));

	// TODO: implement adaptative sampling

	return image;
}
