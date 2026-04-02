#include "Camera.hpp"

Camera::Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal)
	: aspectRatio(aspectRatio), vfov(vfov), imageWidth(imageWidth), samplesPerPixel(samplesPerPixel), maxDepth(maxDepth), cameraCenter(center), normal(normal)
{ initialize(); }

inline float	linearToGamma(float value)
{
	return value > 0 ? sqrt(value) : 0;
}

void	Camera::renderSample(const LinearBVH &geometries, const Materials &materials)
{
	int threads = std::thread::hardware_concurrency();
	vector<std::thread> threadPool;
	int linesPerThread = imageHeight / threads;

	for (int t = 0; t < threads; t++)
	{
		int startLine = t * linesPerThread;
		int endLine = (t == threads - 1) ? imageHeight : startLine + linesPerThread;
		threadPool.emplace_back(&Camera::renderLine, this, startLine, endLine, std::ref(geometries), std::ref(materials));
	}

	for (auto& thread : threadPool)
		thread.join();

	iteration++;
}

vector<int>	Camera::getImage()
{
	vector<int> imageData(imageWidth * imageHeight);

	Limits intensity(0, 0.999);
	for (size_t i = 0; i < buffer.size(); i++)
	{
		Color avgColor = buffer[i] / iteration;
		imageData[i] = (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.x())))) << 16 |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.y())))) << 8 |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.z()))));
	}
	return imageData;
}

void	Camera::initialize()
{
	imageHeight = static_cast<int>(imageWidth / aspectRatio);
	imageHeight = imageHeight < 1 ? 1 : imageHeight;

	buffer.resize(imageWidth * imageHeight);
	iteration = 0;

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

Color	Camera::rayColor(const Ray &ray, const LinearBVH &geometries, const Materials &materials, int depth)
{
	Ray		currentRay = ray;
	Color	accumulatedColor(0, 0, 0);
	Color	throughput(1, 1, 1);

	for (int i = 0; i < depth; i++)
	{
		HitRecord	record;
		Limits	limits(0.001, INFINITY);
		
		if (geometries.hit(currentRay, limits, record))
		{
			ScatterResult	scatterResult = calculateBounce(materials[record.geomIdx], currentRay, record);
			if (scatterResult.scatter)
			{
				throughput *= scatterResult.attenuation;

				float	p = std::min(0.99f, std::max(throughput.x(), std::max(throughput.y(), throughput.z())));
				float	r = Random::global().randomfloat(0, 1);

				if (r > p)
					break;
				else
					throughput /= p;

				currentRay = scatterResult.scatteredRay;
			}
			else
			{
				accumulatedColor += throughput * scatterResult.emmision;
				break;
			}
		}
		else
		{
			Vector3 unitDir = currentRay.getDirection().normalized();
			float t = 0.5 * (unitDir.y() + 1.0);
			Color backgroundColor = (1.0 - t) * Color::DarkGray + t * Color::Black;
			accumulatedColor += throughput * backgroundColor; // Background color
			break;
		}
	}
	return accumulatedColor;
}

void	Camera::renderLine(int start, int end, const LinearBVH &geometries, const Materials &materials)
{
	for (int j = start; j < end; j++)
	{
		for (int i = 0; i < imageWidth; i++)
		{
			Color pixelColor(0, 0, 0);
			Ray ray = getRay(i, j);
			pixelColor += rayColor(ray, geometries, materials, 10);
			buffer[j * imageWidth + i] += pixelColor;
		}
	}
}
