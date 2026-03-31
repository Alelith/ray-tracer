#include "Geometry.hpp"

int	WorldGeometries::totalCount() const
{
	return static_cast<int>(spheres.inverseTransform.size() + planes.inverseTransform.size() + cylinders.inverseTransform.size() +
							cones.inverseTransform.size() + hyperboloids.inverseTransform.size() + tori.inverseTransform.size() +
							kisses.inverseTransform.size() + triangles.v0.size());
}

AABB &WorldGeometries::operator[](int index)
{
	for (size_t i = 0; i < spheres.geomIdx.size(); i++)
	{
		if (spheres.geomIdx[i] == index)
			return spheres.boundingBox[i];
	}

	for (size_t i = 0; i < planes.geomIdx.size(); i++)
	{
		if (planes.geomIdx[i] == index)
			return planes.boundingBox[i];
	}

	for (size_t i = 0; i < cylinders.geomIdx.size(); i++)
	{
		if (cylinders.geomIdx[i] == index)
			return cylinders.boundingBox[i];
	}

	for (size_t i = 0; i < cones.geomIdx.size(); i++)
	{
		if (cones.geomIdx[i] == index)
			return cones.boundingBox[i];
	}

	for (size_t i = 0; i < hyperboloids.geomIdx.size(); i++)
	{
		if (hyperboloids.geomIdx[i] == index)
			return hyperboloids.boundingBox[i];
	}

	for (size_t i = 0; i < tori.geomIdx.size(); i++)
	{
		if (tori.geomIdx[i] == index)
			return tori.boundingBox[i];
	}

	for (size_t i = 0; i < kisses.geomIdx.size(); i++)
	{
		if (kisses.geomIdx[i] == index)
			return kisses.boundingBox[i];
	}

	for (size_t i = 0; i < triangles.geomIdx.size(); i++)
	{
		if (triangles.geomIdx[i] == index)
			return triangles.boundingBox[i];
	}

	throw std::runtime_error("Geometry index not found in WorldGeometries");
}

const AABB &WorldGeometries::operator[](int index) const
{
	for (size_t i = 0; i < spheres.geomIdx.size(); i++)
	{
		if (spheres.geomIdx[i] == index)
			return spheres.boundingBox[i];
	}

	for (size_t i = 0; i < planes.geomIdx.size(); i++)
	{
		if (planes.geomIdx[i] == index)
			return planes.boundingBox[i];
	}

	for (size_t i = 0; i < cylinders.geomIdx.size(); i++)
	{
		if (cylinders.geomIdx[i] == index)
			return cylinders.boundingBox[i];
	}

	for (size_t i = 0; i < cones.geomIdx.size(); i++)
	{
		if (cones.geomIdx[i] == index)
			return cones.boundingBox[i];
	}

	for (size_t i = 0; i < hyperboloids.geomIdx.size(); i++)
	{
		if (hyperboloids.geomIdx[i] == index)
			return hyperboloids.boundingBox[i];
	}

	for (size_t i = 0; i < tori.geomIdx.size(); i++)
	{
		if (tori.geomIdx[i] == index)
			return tori.boundingBox[i];
	}

	for (size_t i = 0; i < kisses.geomIdx.size(); i++)
	{
		if (kisses.geomIdx[i] == index)
			return kisses.boundingBox[i];
	}

	for (size_t i = 0; i < triangles.geomIdx.size(); i++)
	{
		if (triangles.geomIdx[i] == index)
			return triangles.boundingBox[i];
	}

	throw std::runtime_error("Geometry index not found in WorldGeometries");
}

Ray	getLocalRay(const Matrix4x4 &inverse, const Ray &ray)
{
	const Point3	&wo = ray.getOrigin();
	return Ray(inverse * wo,
			   inverse * ray.getDirection());
}

bool	geometryHit(const WorldGeometries &g, const Ray &ray, const Limits &limits, HitRecord &record)
{
	HitRecord	tmpRecord;
	bool		hitAnything = false;
	Limits		localLimits = limits;

	if (spheresHit(g.spheres, ray, localLimits, tmpRecord))
	{
		hitAnything = true;
		record = tmpRecord;
	}
	return hitAnything;
}
