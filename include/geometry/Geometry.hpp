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

enum class GeometryType : uint8_t
{
	Sphere,
	Plane,
	Cylinder,
	Cone,
	Hyperboloid,
	Torus,
	Kiss,
	Triangle
};

struct SimpleGeometryData
{
	vector<Matrix4x4>		inverseTransform;
	vector<Matrix4x4>		transposeInverseTransform;
	vector<AABB>			boundingBox;
};

struct ParametrizedGeometryData
{
	vector<Matrix4x4>		inverseTransform;
	vector<Matrix4x4>		transposeInverseTransform;
	vector<float>			param;
	vector<AABB>			boundingBox;
};

struct WorldGeometryData
{
	vector<Point3>			v0;
	vector<Point3>			v1;
	vector<Point3>			v2;
	vector<Vector3>			triNormal;
	vector<AABB>			boundingBox;
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
};

struct GeometryData
{
	GeometryType	type;
	Transform		transform;		// Unused for Triangle
	AABB			boundingBox;	// Precomputed AABB in local space for broad-phase culling

	float			thickness;		// Torus:       minorRadius / majorRadius
	float			waist;			// Hyperboloid: waist parameter
	float			sharpness;		// Kiss:        sharpness parameter

	Point3			v0;				// Triangle:    vertex 0
	Point3			v1;				// Triangle:    vertex 1
	Point3			v2;				// Triangle:    vertex 2
	Vector3			triNormal;		// Triangle:    face normal
};

Ray				getLocalRay(const Transform &transform, const Ray &ray);

void			makeSphere      (const Point3 &center, float radius, WorldGeometries &worldGeoms);
void			makePlane       (const Point3 &point, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeHyperboloid (float waist, float radius, float height, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTorus       (float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeKiss        (float sharpness, const Point3 &center, const Vector3 &normal, WorldGeometries &worldGeoms);
void			makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, WorldGeometries &worldGeoms);

GeometryData	makeSphere      (const Point3 &center, float radius);
GeometryData	makePlane       (const Point3 &point, const Vector3 &normal);
GeometryData	makeCylinder    (float radius, float height, const Point3 &center, const Vector3 &normal);
GeometryData	makeCone        (float radius, float height, const Point3 &center, const Vector3 &normal);
GeometryData	makeHyperboloid (float waist, float radius, float height, const Point3 &center, const Vector3 &normal);
GeometryData	makeTorus       (float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal);
GeometryData	makeKiss        (float sharpness, const Point3 &center, const Vector3 &normal);
GeometryData	makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal);

bool			sphereHit      (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			planeHit       (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			cylinderHit    (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			coneHit        (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			hyperboloidHit (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			torusHit       (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			kissHit        (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			triangleHit    (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);

bool			geometryHit    (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);

#endif /* GEOMETRY_HPP */
