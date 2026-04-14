#include "Geometry.cuh"

namespace geometry
{

	WorldGeometries	WorldGeometries::cudaCopy() const
	{
		WorldGeometries	cudaData;
		cudaData.count = count;
		if (count > 0)
		{
			cudaMalloc(&cudaData.primitives, count * sizeof(Primitive));
			cudaMemcpy(cudaData.primitives, primitives, count * sizeof(Primitive), cudaMemcpyHostToDevice);
		}
		return cudaData;
	}
	
	void	WorldGeometries::cudaFreeWorld()
	{
		if (count > 0)
			cudaFree(primitives);
	}
	
	RT_CALLABLE Ray	getLocalRay(const Matrix4x4 &inverse, const Ray &ray)
	{
		const Point3	&wo = ray.getOrigin();
		return Ray(inverse * wo, inverse * ray.getDirection());
	}

} // namespace geometry
