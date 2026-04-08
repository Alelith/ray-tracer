#include "Camera.cuh"
#include <iostream>

RT_CALLABLE Color	rayColorKernel(Ray ray, Random *random, int pixelIndex, RenderData *data)
{
	Ray		currentRay = ray;
	Color	accumulatedColor(0, 0, 0);
	Color	throughput(1, 1, 1);

	for (int i = 0; i < data->maxDepth; i++)
	{
		HitRecord	record;
		Limits	limits(0.001, INFINITY);
		
		if (data->linearbvh->hit(currentRay, limits, record))
		{
			if (i == 0)
			{
				data->normalBuffer[pixelIndex] = record.normal;
				data->positionBuffer[pixelIndex] = record.point;
				data->albedoBuffer[pixelIndex] = data->materials[record.geomIdx].albedo;
			}

			ScatterResult	scatterResult = calculateBounce(data->materials[record.geomIdx], currentRay, record, random);
			if (scatterResult.scatter)
			{
				throughput *= scatterResult.attenuation;

				float	p = fminf(0.99f, fmaxf(throughput.x(), fmaxf(throughput.y(), throughput.z())));
				float	r = random->randomfloat();

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
			if (i == 0)
			{
				data->normalBuffer[pixelIndex] = Vector3(0, 0, 0);
				data->positionBuffer[pixelIndex] = Point3(0, 0, 0);
				data->albedoBuffer[pixelIndex] = Color(0, 0, 0);
			}

			Vector3 unitDir = currentRay.getDirection().normalized();
			float t = 0.5 * (unitDir.y() + 1.0);
			Color backgroundColor = (1.0 - t) * Color(0.25, 0.25, 0.25) + t * Color(0, 0, 0);
			accumulatedColor += throughput * backgroundColor; // Background color
			break;
		}
	}
	return accumulatedColor;
}

RT_KERNEL void	renderKernel(RenderData *data, int iteration)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;

	if (i >= data->width || j >= data->height)
		return;
	int pixelIndex = j * data->width + i;

	Random random(pixelIndex + (iteration * data->width * data->height)); // Simple per-pixel random seed

	Ray ray = data->getRay(i, j);

	Color pixelColor(0, 0, 0);
	pixelColor += rayColorKernel(ray, &random, pixelIndex, data);
	data->buffer[pixelIndex] += pixelColor;
}

RT_CALLABLE Ray	RenderData::getRay(float u, float v) const
{
	float   provisionalRandom = 0.5f;
	Vector3	offset(provisionalRandom - 0.5, provisionalRandom - 0.5, 0);
	Point3	rayOrigin = pixel00Loc + (u + offset[0]) * pixelDeltaU + (v + offset[1]) * pixelDeltaV;
	Vector3	rayDir = rayOrigin - cameraCenter;
	return Ray(cameraCenter, rayDir);
}

Camera::Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal, const LinearBVH &geometries, const Materials &materials)
	: aspectRatio(aspectRatio), vfov(vfov), imageWidth(imageWidth), samplesPerPixel(samplesPerPixel), maxDepth(maxDepth), cameraCenter(center), normal(normal)
{ initialize(geometries, materials); }

Camera::~Camera()
{
	cudaFree(cudaRenderData.buffer);
	cudaFree(cudaRenderData.pingPongBuffer);
	cudaFree(cudaRenderData.normalBuffer);
	cudaFree(cudaRenderData.positionBuffer);
	cudaFree(cudaRenderData.albedoBuffer);
	LinearBVH hostBVH;
	cudaMemcpy(&hostBVH, cudaRenderData.linearbvh, sizeof(LinearBVH), cudaMemcpyDeviceToHost);
	hostBVH.cudaFreeBVH();
	cudaFree(cudaRenderData.linearbvh);
	cudaFree(cudaRenderData.materials);
	cudaFree(allocatedCudaRenderData);
}

inline float	linearToGamma(float value)
{
	return value > 0 ? sqrt(value) : 0;
}

void	Camera::renderSample()
{
	dim3 blockSize(16, 16);
	dim3 gridSize((imageWidth + blockSize.x - 1) / blockSize.x, (imageHeight + blockSize.y - 1) / blockSize.y);

	renderKernel<<<gridSize, blockSize>>>(allocatedCudaRenderData, iteration);
	cudaDeviceSynchronize();

	iteration++;
}

vector<int>	Camera::getImage()
{
	vector<int> imageData(imageWidth * imageHeight);
	vector<Color> buffer(imageWidth * imageHeight);
	cudaMemcpy(buffer.data(), cudaRenderData.buffer, imageWidth * imageHeight * sizeof(Color), cudaMemcpyDeviceToHost);

	Limits intensity(0, 0.999);
	for (size_t i = 0; i < static_cast<size_t>(imageWidth * imageHeight); i++)
	{
		Color avgColor = buffer[i] / iteration;
		imageData[i] = (255 << 24) |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.x())))) << 16 |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.y())))) << 8 |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.z()))));
	}
	return imageData;
}

void	Camera::initialize(const LinearBVH &geometries, const Materials &materials)
{
	imageHeight = static_cast<int>(imageWidth / aspectRatio);
	imageHeight = imageHeight < 1 ? 1 : imageHeight;

	iteration = 0;

	pixelSamplesScale = 1.0 / samplesPerPixel;

	float	focalLength = 1.0;
	float	theta = vfov * M_PI / 180.0;
	float	h = tan(theta / 2);
	float	viewportHeight = 2.0 * h * focalLength;
	float	viewportWidth = aspectRatio * viewportHeight;

	Vector3	w = -normal.normalized();
	Vector3	u = cross(Vector3(0, 1, 0), w);
	float	uLength = u.length();
	if (uLength < 1e-6)
		u = cross(Vector3(1, 0, 0), w).normalized();
	else
		u = u / uLength;
	Vector3	v = cross(w, u);

	Vector3	viewportU = viewportWidth * u;
	Vector3	viewportV = viewportHeight * -v;

	pixelDeltaU = viewportU / imageWidth;
	pixelDeltaV = viewportV / imageHeight;

	Point3	viewportUpperLeft = cameraCenter - focalLength * w - viewportU / 2 - viewportV / 2;
	pixel00Loc = viewportUpperLeft + 0.5 * pixelDeltaU + 0.5 * pixelDeltaV;

	cudaRenderData.width = imageWidth;
	cudaRenderData.height = imageHeight;
	cudaRenderData.maxDepth = maxDepth;
	cudaRenderData.cameraCenter = cameraCenter;
	cudaRenderData.pixel00Loc = pixel00Loc;
	cudaRenderData.pixelDeltaU = pixelDeltaU;
	cudaRenderData.pixelDeltaV = pixelDeltaV;

	Color *d_buffer;
	Color *d_pingPongBuffer;
	Vector3 *d_normalBuffer;
	Point3 *d_positionBuffer;
	Color *d_albedoBuffer;
	cudaMalloc(&d_buffer, imageWidth * imageHeight * sizeof(Color));
	cudaMalloc(&d_pingPongBuffer, imageWidth * imageHeight * sizeof(Color));
	cudaMalloc(&d_normalBuffer, imageWidth * imageHeight * sizeof(Vector3));
	cudaMalloc(&d_positionBuffer, imageWidth * imageHeight * sizeof(Point3));
	cudaMalloc(&d_albedoBuffer, imageWidth * imageHeight * sizeof(Color));
	cudaRenderData.buffer = d_buffer;
	cudaRenderData.pingPongBuffer = d_pingPongBuffer;
	cudaRenderData.normalBuffer = d_normalBuffer;
	cudaRenderData.positionBuffer = d_positionBuffer;
	cudaRenderData.albedoBuffer = d_albedoBuffer;
	cudaMemset(cudaRenderData.buffer, 0, imageWidth * imageHeight * sizeof(Color));

	LinearBVH *d_linearbvh;
	cudaMalloc(&d_linearbvh, sizeof(LinearBVH));
	LinearBVH deviceBVH = geometries.cudaCopy();
	cudaMemcpy(d_linearbvh, &deviceBVH, sizeof(LinearBVH), cudaMemcpyHostToDevice);
	cudaRenderData.linearbvh = d_linearbvh;

	Material *d_materials;
	cudaMalloc(&d_materials, materials.count * sizeof(Material));
	cudaMemcpy(d_materials, materials.materials, materials.count * sizeof(Material), cudaMemcpyHostToDevice);
	cudaRenderData.materials = d_materials;

	cudaMalloc(&allocatedCudaRenderData, sizeof(RenderData));
	cudaMemcpy(allocatedCudaRenderData, &cudaRenderData, sizeof(RenderData), cudaMemcpyHostToDevice);
}
