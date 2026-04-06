#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include "Solver.hpp"
# include "AABB.hpp"
# include "HitRecord.hpp"
# include "Limits.hpp"
# include <cstdint>

struct SimpleGeometryData
{
	Matrix4x4	*inverseTransform = nullptr;
	AABB		*boundingBox = nullptr;
	int			*geomIdx = nullptr; // Index to identify which geometry was hit in the original list
	size_t		count = 0;
};

struct ParametrizedGeometryData
{
	Matrix4x4	*inverseTransform = nullptr;
	float		*param = nullptr;
	AABB		*boundingBox = nullptr;
	int			*geomIdx = nullptr; // Index to identify which geometry was hit in the original list
	size_t		count = 0;
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
};

RT_CALLABLE Ray		getLocalRay     (const Matrix4x4 &inverse, const Ray &ray);

RT_CALLABLE void	makeSphere      (const Point3 &center, float radius, WorldGeometries &worldGeoms);
/*void			makePlane       (const Point3 &point, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeHyperboloid (float waist, float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTorus       (float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeKiss        (float sharpness, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, WorldGeometries &worldGeoms);*/

RT_CALLABLE bool	sphereHits      (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);

#endif /* GEOMETRY_HPP */
