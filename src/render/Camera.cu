#include "Camera.cuh"
#include <iostream>

RT_KERNEL void	aTorusKernel(RenderData *data, Color *read, Color *write, int stepWidth)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;

	if (i >= data->width || j >= data->height)
		return;

	int pixelIdx = j * data->width + i;
	Point3 center = data->positionBuffer[pixelIdx];
	Vector3 normal = data->normalBuffer[pixelIdx];
	Color albedo = data->albedoBuffer[pixelIdx];

	Color currentColor = read[pixelIdx].clamp(0, 1);

	Color accumulatedColor(0, 0, 0);
	float weightSum = 0.0f;
	for (int dy = -stepWidth; dy <= stepWidth; dy += stepWidth)
	{
		for (int dx = -stepWidth; dx <= stepWidth; dx += stepWidth)
		{
			int neighborX = i + dx;
			int neighborY = j + dy;

			if (neighborX < 0 || neighborX >= data->width || neighborY < 0 || neighborY >= data->height)
				continue;

			int neighborIdx = neighborY * data->width + neighborX;
			Point3 neighborPos = data->positionBuffer[neighborIdx];
			Vector3 neighborNormal = data->normalBuffer[neighborIdx];
			Color neighborAlbedo = data->albedoBuffer[neighborIdx];
			Color neighborColor = read[neighborIdx];
			Color clampedNeighborColor = neighborColor.clamp(0, 1);

			float Wn = powf(fmaxf(0.0f, dot(normal, neighborNormal)), data->normalWeight);
			float Wc = expf(-(center - neighborPos).lengthSqrd() / data->positionWeight);
			float Wa = expf(-(albedo - neighborAlbedo).lengthSqrd() / data->albedoWeight);

			float Wcolor = expf(-(currentColor - clampedNeighborColor).lengthSqrd() / data->colorWeight);

			float Ws;
			if (dx == 0 && dy == 0)
				Ws = 1.0 / 4.0;
			else if (abs(dx - dy) == 0 || abs(dx - dy) == stepWidth * 2)
				Ws = 1.0 / 16.0;
			else
				Ws = 1.0 / 8.0;

			float weight = Ws * Wn * Wc * Wa * Wcolor;

			accumulatedColor += neighborColor * weight;
			weightSum += weight;
		}
	}
	write[pixelIdx] = (weightSum > 0) ? (accumulatedColor / weightSum) : read[pixelIdx];
}

RT_KERNEL void	prepareFilterKernel(RenderData *data, int iteration)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;

	if (i >= data->width || j >= data->height)
		return;
	int pixelIndex = j * data->width + i;

	data->colorBuffer[pixelIndex] = data->buffer[pixelIndex] / iteration;
}

__device__ Color	rayColorKernel(const Ray &ray, Random &random, int pixelIndex, RenderData *data, int iteration)
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
			if (i == 0 && iteration == 0)
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
				float	r = random.randomfloat();

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
			if (i == 0 && iteration == 0)
			{
				data->normalBuffer[pixelIndex] = Vector3(0, 0, 0);
				data->positionBuffer[pixelIndex] = Point3(0, 0, 0);
				data->albedoBuffer[pixelIndex] = Color(0, 0, 0);
			}

			Vector3 unitDir = currentRay.getDirection().normalized();
			float t = 0.5 * (unitDir.y() + 1.0);
			Color backgroundColor = (1.0 - t) * Color(0.4, 0.4, 0.6) + t * Color(0.1, 0.1, 0.2);
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

	Ray ray = data->getRay(i, j, random, iteration);

	Color pixelColor(0, 0, 0);
	pixelColor += rayColorKernel(ray, random, pixelIndex, data, iteration);
	data->buffer[pixelIndex] += pixelColor;
}

RT_CALLABLE Ray	RenderData::getRay(float u, float v, Random &rand, int iteration) const
{
	Vector3	offset(rand.randomfloat() - 0.5f, rand.randomfloat() - 0.5f, 0);
	if (iteration == 0)
		offset = Vector3(0, 0, 0);
	Point3	rayOrigin = pixel00Loc + (u + offset[0]) * pixelDeltaU + (v + offset[1]) * pixelDeltaV;
	Vector3	rayDir = rayOrigin - cameraCenter;
	return Ray(cameraCenter, rayDir);
}

Camera::Camera(float aspectRatio, int imageWidth, int samplesPerPixel, int maxDepth, float vfov, const Point3 &center, const Vector3 &normal, const LinearBVH &geometries, const Materials &materials, float normalWeight, float positionWeight, float albedoWeight, float colorWeight)
	: aspectRatio(aspectRatio), vfov(vfov), imageWidth(imageWidth), samplesPerPixel(samplesPerPixel), maxDepth(maxDepth), cameraCenter(center), normal(normal)
{ initialize(geometries, materials, normalWeight, positionWeight, albedoWeight, colorWeight); }

Camera::~Camera()
{
	cudaFree(cudaRenderData.buffer);
	cudaFree(cudaRenderData.pingPongBuffer);
	cudaFree(cudaRenderData.colorBuffer);
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

	prepareFilterKernel<<<gridSize, blockSize>>>(allocatedCudaRenderData, iteration + 1);
	cudaDeviceSynchronize();

	for (int i = 0; i < 5; i++)
	{
		Color *write = i % 2 == 0 ? cudaRenderData.pingPongBuffer : cudaRenderData.colorBuffer;
		Color *read = i % 2 == 0 ? cudaRenderData.colorBuffer : cudaRenderData.pingPongBuffer;

		int stepWidth = i == 0 ? 1 : (1 << i);
		aTorusKernel<<<gridSize, blockSize>>>(allocatedCudaRenderData, read, write, stepWidth);
		cudaDeviceSynchronize();
	}

	iteration++;
}

vector<int>	Camera::getImage()
{
	vector<int> imageData(imageWidth * imageHeight);
	vector<Color> buffer(imageWidth * imageHeight);
	cudaMemcpy(buffer.data(), cudaRenderData.pingPongBuffer, imageWidth * imageHeight * sizeof(Color), cudaMemcpyDeviceToHost);

	Limits intensity(0, 0.999);
	for (size_t i = 0; i < static_cast<size_t>(imageWidth * imageHeight); i++)
	{
		Color avgColor = buffer[i];
		imageData[i] = (255 << 24) |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.x())))) << 16 |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.y())))) << 8 |
					   (static_cast<int>(256 * intensity.clamp(linearToGamma(avgColor.z()))));
	}
	return imageData;
}

void	Camera::initialize(const LinearBVH &geometries, const Materials &materials, float normalWeight, float positionWeight, float albedoWeight, float colorWeight)
{
	imageHeight = static_cast<int>(imageWidth / aspectRatio);
	imageHeight = imageHeight < 1 ? 1 : imageHeight;

	iteration = 0;

	pixelSamplesScale = 1.0 / samplesPerPixel;

	float	focalLength = 1.0;
	float	theta = vfov * M_PI_F / 180.0;
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
	Color *d_colorBuffer;
	Vector3 *d_normalBuffer;
	Point3 *d_positionBuffer;
	Color *d_albedoBuffer;
	cudaMalloc(&d_buffer, imageWidth * imageHeight * sizeof(Color));
	cudaMalloc(&d_pingPongBuffer, imageWidth * imageHeight * sizeof(Color));
	cudaMalloc(&d_colorBuffer, imageWidth * imageHeight * sizeof(Color));
	cudaMalloc(&d_normalBuffer, imageWidth * imageHeight * sizeof(Vector3));
	cudaMalloc(&d_positionBuffer, imageWidth * imageHeight * sizeof(Point3));
	cudaMalloc(&d_albedoBuffer, imageWidth * imageHeight * sizeof(Color));
	cudaRenderData.buffer = d_buffer;
	cudaRenderData.pingPongBuffer = d_pingPongBuffer;
	cudaRenderData.colorBuffer = d_colorBuffer;
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

	cudaRenderData.normalWeight = normalWeight;
	cudaRenderData.positionWeight = positionWeight;
	cudaRenderData.albedoWeight = albedoWeight;
	cudaRenderData.colorWeight = colorWeight;

	cudaMalloc(&allocatedCudaRenderData, sizeof(RenderData));
	cudaMemcpy(allocatedCudaRenderData, &cudaRenderData, sizeof(RenderData), cudaMemcpyHostToDevice);
}
