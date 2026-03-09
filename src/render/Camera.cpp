#include "Camera.hpp"

Camera::Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal)
	: aspectRatio(aspectRatio), vfov(vfov), imageWidth(imageWidth), samplesPerPixel(samplesPerPixel), maxDepth(maxDepth), cameraCenter(center), normal(normal)
{ initialize(); }

inline float	linearToGamma(float value)
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

void	Camera::render(const WorldGeometries &geometries, const Materials &materials)
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
				pixelColor += rayColor(ray, geometries, materials, 10);
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

	float	focalLength = 1.0;
	float	theta = vfov * M_PI / 180.0;
	float	h = tan(theta / 2);
	float	viewportHeight = 2.0 * h * focalLength;
	float	viewportWidth = aspectRatio * viewportHeight;

	Vector3	w = -normal.normalized();
	Vector3	u = cross(Vector3::Up, w);
	float	uLength = u.length();
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

Ray	Camera::getRay(float u, float v) const
{
	Random&	random = Random::global();
	Vector3	offset(random.randomfloat(0, 1) -0.5, random.randomfloat(0, 1) - 0.5, 0);
	auto	rayOrigin = pixel00Loc + (u + offset[0]) * pixelDeltaU + (v + offset[1]) * pixelDeltaV;
	auto	rayDir = rayOrigin - cameraCenter;
	return Ray(cameraCenter, rayDir);
}

Color	Camera::rayColor(const Ray &ray, const WorldGeometries &geometries, const Materials &materials, int depth)
{
	Ray		currentRay = ray;
	Color	accumulatedColor(0, 0, 0);
	Color	throughput(1, 1, 1);

	for (int i = 0; i < depth; i++)
	{
		HitRecord	record;
		Limits	limits(0.001, INFINITY);
		
		if (geometryHit(geometries, currentRay, limits, record))
		{
			ScatterResult	scatterResult = calculateBounce(currentRay, record, materials);
			if (scatterResult.scatter)
			{
				throughput *= scatterResult.attenuation;
				currentRay = scatterResult.scatteredRay;
			}
			else
			{
				accumulatedColor += throughput * Color(0, 0, 0); // Absorbed, contributes no light
				break;
			}
		}
		else
		{
			Vector3 unitDir = currentRay.getDirection().normalized();
			float t = 0.5 * (unitDir.y() + 1.0);
			Color backgroundColor = (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
			accumulatedColor += throughput * backgroundColor; // Background color
			break;
		}
	}
	return accumulatedColor;
}
