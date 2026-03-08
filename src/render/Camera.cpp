#include "Camera.hpp"

Camera::Camera(double aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, double vfov, const Point3 &center, const Vector3 &normal)
	: aspectRatio(aspectRatio), vfov(vfov), imageWidth(imageWidth), samplesPerPixel(samplesPerPixel), maxDepth(maxDepth), cameraCenter(center), normal(normal)
{ initialize(); }

inline double	linearToGamma(double value)
{
	return value > 0 ? sqrt(value) : 0;
}

static void	writeColor(std::ostream &out, const Color &color)
{
	static const Limits	intensity(0, 0.999);
	int ir = static_cast<int>(256 * intensity.clamp(linearToGamma(color[0])));
	int ig = static_cast<int>(256 * intensity.clamp(linearToGamma(color[1])));
	int ib = static_cast<int>(256 * intensity.clamp(linearToGamma(color[2])));
	out << ir << ' ' << ig << ' ' << ib << '\n';
}

void	Camera::render(const GeometryData *geoms, const MaterialData *materials, int geomCount)
{
	std::cout << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

	for (int j = 0; j < imageHeight; j++)
	{
		for (int i = 0; i < imageWidth; i++)
		{
			Color pixelColor(0, 0, 0);
			for (int s = 0; s < samplesPerPixel; s++)
			{
				Ray ray = getRay(i, j);
				pixelColor += rayColor(ray, geoms, materials, geomCount, 10);
			}
			pixelColor *= pixelSamplesScale;
			writeColor(std::cout, pixelColor);
		}
	}
}

void	Camera::initialize()
{
	imageHeight = static_cast<int>(imageWidth / aspectRatio);
	imageHeight = imageHeight < 1 ? 1 : imageHeight;

	pixelSamplesScale = 1.0 / samplesPerPixel;

	double	focalLength = 1.0;
	double	theta = vfov * M_PI / 180.0;
	double	h = tan(theta / 2);
	double	viewportHeight = 2.0 * h * focalLength;
	double	viewportWidth = aspectRatio * viewportHeight;

	Vector3	w = -normal.normalized();
	Vector3	u = cross(Vector3::Up, w);
	double	uLength = u.length();
	if (uLength < 1e-6)
		u = cross(Vector3::Right, w).normalized();
	else
		u = u / uLength;
	Vector3	v = cross(w, u);

	Vector3	viewportU = viewportWidth * u;
	Vector3	viewportV = viewportHeight * -v;

	pixelDeltaU = viewportU / imageWidth;
	pixelDeltaV = viewportV / imageHeight;

	Point3	viewportUpperLeft = cameraCenter - focalLength * w - viewportU / 2 - viewportV / 2;
	pixel00Loc = viewportUpperLeft + 0.5 * pixelDeltaU + 0.5 * pixelDeltaV;
}

Ray	Camera::getRay(double u, double v) const
{
	Random&	random = Random::global();
	Vector3	offset(random.randomDouble(0, 1) -0.5, random.randomDouble(0, 1) - 0.5, 0);
	auto	rayOrigin = pixel00Loc + (u + offset[0]) * pixelDeltaU + (v + offset[1]) * pixelDeltaV;
	auto	rayDir = rayOrigin - cameraCenter;
	return Ray(cameraCenter, rayDir);
}

static bool	hitAnything(const Ray &ray, const Limits &limits, const GeometryData *geoms, int geomCount, HitRecord &record)
{
	HitRecord	tempRecord;
	bool		hitAnything = false;
	double		closestSoFar = limits.getMax();

	for (int i = 0; i < geomCount; i++)
	{
		if (geometryHit(geoms[i], ray, Limits(limits.getMin(), closestSoFar), tempRecord))
		{
			hitAnything = true;
			closestSoFar = tempRecord.t;
			record = tempRecord;
			record.geomIdx = i; // Store the index of the hit geometry
		}
	}

	return hitAnything;
}

Color	Camera::rayColor(const Ray &ray, const GeometryData *geoms, const MaterialData *materials, int geomCount, int depth)
{
	if (depth <= 0)
		return Color(0, 0, 0);

	HitRecord	record;

	if (hitAnything(ray, Limits(0.0001, INFINITY), geoms, geomCount, record))
	{
		Ray		scattered;
		Color	attenuation;
		if (scatter(ray, materials[record.geomIdx], record, attenuation, scattered))
			return attenuation * rayColor(scattered, geoms, materials, geomCount, depth - 1);
		return Color(0, 0, 0);
	}
	Vector3 unitDir = ray.getDirection().normalized();
	double t = 0.5 * (unitDir[1] + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}
