#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include "Solver.hpp"
# include "AABB.hpp"
# include "Transform.hpp"
# include "HitRecord.hpp"
# include "Limits.hpp"
# include <cstdint>
# include <vector>

using std::vector;

struct SimpleGeometryData
{
	vector<Matrix4x4>		inverseTransform;
	vector<Matrix4x4>		transposeInverseTransform;
	vector<AABB>			boundingBox;
	vector<int>				geomIdx; // Index to identify which geometry was hit in the original list
};

struct ParametrizedGeometryData
{
	vector<Matrix4x4>		inverseTransform;
	vector<Matrix4x4>		transposeInverseTransform;
	vector<float>			param;
	vector<AABB>			boundingBox;
	vector<int>				geomIdx; // Index to identify which geometry was hit in the original list
};

struct WorldGeometryData
{
	vector<Point3>			v0;
	vector<Point3>			v1;
	vector<Point3>			v2;
	vector<Vector3>			triNormal;
	vector<AABB>			boundingBox;
	vector<int>				geomIdx; // Index to identify which geometry was hit in the original list
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

	int	totalCount() const;

	AABB &operator[](int index);

	const AABB &operator[](int index) const;
};

Ray				getLocalRay     (const Matrix4x4 &inverse, const Ray &ray);

void			makeSphere      (const Point3 &center, float radius, WorldGeometries &worldGeoms);
/*void			makePlane       (const Point3 &point, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeHyperboloid (float waist, float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTorus       (float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeKiss        (float sharpness, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, WorldGeometries &worldGeoms);*/

bool			sphereHits      (const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
bool			spheresHit      (const SimpleGeometryData &spheres, const Ray &ray, Limits &limits, HitRecord &record);

bool			geometryHit     (const WorldGeometries &g, const Ray &ray, const Limits &limits, HitRecord &record);

#endif /* GEOMETRY_HPP */
