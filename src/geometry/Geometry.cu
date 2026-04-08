#include "Geometry.cuh"

SimpleGeometryData	SimpleGeometryData::cudaCopy() const
{
	SimpleGeometryData cudaData;
	cudaData.count = count;

	if (count > 0)
	{
		cudaMalloc(&cudaData.inverseTransform, count * sizeof(Matrix4x4));
		cudaMemcpy(cudaData.inverseTransform, inverseTransform, count * sizeof(Matrix4x4), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.boundingBox, count * sizeof(AABB));
		cudaMemcpy(cudaData.boundingBox, boundingBox, count * sizeof(AABB), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.geomIdx, count * sizeof(int));
		cudaMemcpy(cudaData.geomIdx, geomIdx, count * sizeof(int), cudaMemcpyHostToDevice);
	}

	return cudaData;
}

void	SimpleGeometryData::cudaFreeSimple()
{
	cudaFree(inverseTransform);
	cudaFree(boundingBox);
	cudaFree(geomIdx);
}

ParametrizedGeometryData	ParametrizedGeometryData::cudaCopy() const
{
	ParametrizedGeometryData cudaData;
	cudaData.count = count;

	if (count > 0)
	{
		cudaMalloc(&cudaData.inverseTransform, count * sizeof(Matrix4x4));
		cudaMemcpy(cudaData.inverseTransform, inverseTransform, count * sizeof(Matrix4x4), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.param, count * sizeof(float));
		cudaMemcpy(cudaData.param, param, count * sizeof(float), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.boundingBox, count * sizeof(AABB));
		cudaMemcpy(cudaData.boundingBox, boundingBox, count * sizeof(AABB), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.geomIdx, count * sizeof(int));
		cudaMemcpy(cudaData.geomIdx, geomIdx, count * sizeof(int), cudaMemcpyHostToDevice);
	}

	return cudaData;
}

void	ParametrizedGeometryData::cudaFreeParametrized()
{
	cudaFree(inverseTransform);
	cudaFree(param);
	cudaFree(boundingBox);
	cudaFree(geomIdx);
}

WorldGeometryData	WorldGeometryData::cudaCopy() const
{
	WorldGeometryData cudaData;
	cudaData.count = count;

	if (count > 0)
	{
		cudaMalloc(&cudaData.v0, count * sizeof(Point3));
		cudaMemcpy(cudaData.v0, v0, count * sizeof(Point3), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.v1, count * sizeof(Point3));
		cudaMemcpy(cudaData.v1, v1, count * sizeof(Point3), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.v2, count * sizeof(Point3));
		cudaMemcpy(cudaData.v2, v2, count * sizeof(Point3), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.triNormal, count * sizeof(Vector3));
		cudaMemcpy(cudaData.triNormal, triNormal, count * sizeof(Vector3), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.boundingBox, count * sizeof(AABB));
		cudaMemcpy(cudaData.boundingBox, boundingBox, count * sizeof(AABB), cudaMemcpyHostToDevice);

		cudaMalloc(&cudaData.geomIdx, count * sizeof(int));
		cudaMemcpy(cudaData.geomIdx, geomIdx, count * sizeof(int), cudaMemcpyHostToDevice);
	}

	return cudaData;
}

void	WorldGeometryData::cudaFreeWorld()
{
	cudaFree(v0);
	cudaFree(v1);
	cudaFree(v2);
	cudaFree(triNormal);
	cudaFree(boundingBox);
	cudaFree(geomIdx);
}

RT_CALLABLE size_t	WorldGeometries::totalCount() const
{
	return spheres.count + planes.count + cylinders.count + cones.count +
		   hyperboloids.count + tori.count + kisses.count + triangles.count;
}

WorldGeometries	WorldGeometries::cudaCopy() const
{
	WorldGeometries cudaWorld;

	cudaWorld.spheres = spheres.cudaCopy();
	cudaWorld.planes = planes.cudaCopy();
	cudaWorld.cylinders = cylinders.cudaCopy();
	cudaWorld.cones = cones.cudaCopy();
	cudaWorld.hyperboloids = hyperboloids.cudaCopy();
	cudaWorld.tori = tori.cudaCopy();
	cudaWorld.kisses = kisses.cudaCopy();
	cudaWorld.triangles = triangles.cudaCopy();

	return cudaWorld;
}

void	WorldGeometries::cudaFreeWorld()
{
	spheres.cudaFreeSimple();
	planes.cudaFreeSimple();
	cylinders.cudaFreeSimple();
	cones.cudaFreeSimple();
	hyperboloids.cudaFreeParametrized();
	tori.cudaFreeParametrized();
	kisses.cudaFreeParametrized();
	triangles.cudaFreeWorld();
}

RT_CALLABLE Ray	getLocalRay(const Matrix4x4 &inverse, const Ray &ray)
{
	const Point3	&wo = ray.getOrigin();
	return Ray(inverse * wo,
			   inverse * ray.getDirection());
}
