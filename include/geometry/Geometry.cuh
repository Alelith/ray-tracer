#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include "Solver.cuh"
# include "AABB.cuh"
# include "HitRecord.cuh"
# include "Limits.cuh"
# include <cstdint>

struct SimpleGeometryData
{
	Matrix4x4	*inverseTransform = nullptr;
	AABB		*boundingBox = nullptr;
	int			*geomIdx = nullptr; // Index to identify which geometry was hit in the original list
	size_t		count = 0;

	SimpleGeometryData	cudaCopy() const;
	void				cudaFreeSimple();
};

struct ParametrizedGeometryData
{
	Matrix4x4	*inverseTransform = nullptr;
	float		*param = nullptr;
	AABB		*boundingBox = nullptr;
	int			*geomIdx = nullptr; // Index to identify which geometry was hit in the original list
	size_t		count = 0;

	ParametrizedGeometryData	cudaCopy() const;
	void						cudaFreeParametrized();
};

struct WorldGeometryData
{
	Point3	*v0 = nullptr;
	Point3	*v1 = nullptr;
	Point3	*v2 = nullptr;
	Vector3	*triNormal = nullptr;
	AABB	*boundingBox = nullptr;
	int		*geomIdx = nullptr; // Index to identify which geometry was hit in the original list
	size_t	count = 0;

	WorldGeometryData	cudaCopy() const;
	void				cudaFreeWorld();
};

struct WorldGeometries
{
	SimpleGeometryData			spheres;
	SimpleGeometryData			planes;
	SimpleGeometryData			cylinders;
	SimpleGeometryData			cones;
	ParametrizedGeometryData	hyperboloids;
	ParametrizedGeometryData	tori;
	ParametrizedGeometryData	kisses;
	WorldGeometryData			triangles;

	RT_CALLABLE size_t	totalCount() const;

	WorldGeometries		cudaCopy() const;
	void				cudaFreeWorld();
};

RT_CALLABLE Ray		getLocalRay     (const Matrix4x4 &inverse, const Ray &ray);

RT_CALLABLE void	makeSphere      (const Point3 &center, float radius, WorldGeometries &worldGeoms);
RT_CALLABLE void	makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
RT_CALLABLE void	makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
RT_CALLABLE void	makeTorus       (float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
RT_CALLABLE void	makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, WorldGeometries &worldGeoms);
/*void			makePlane       (const Point3 &point, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeHyperboloid (float waist, float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeKiss        (float sharpness, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, WorldGeometries &worldGeoms);*/

RT_CALLABLE bool	sphereHits      (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
RT_CALLABLE bool	cylinderHits    (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
RT_CALLABLE bool	coneHits        (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
RT_CALLABLE bool	torusHits       (const Matrix4x4 &inverseTransform, float param, const Ray &ray, const Limits &limits, HitRecord &record);
RT_CALLABLE bool	triangleHits    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, const Ray &ray, const Limits &limits, HitRecord &record);

#endif /* GEOMETRY_HPP */
