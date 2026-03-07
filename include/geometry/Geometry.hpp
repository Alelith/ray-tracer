#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include "Solver.hpp"
# include "AABB.hpp"
# include "Transform.hpp"
# include "HitRecord.hpp"
# include "Limits.hpp"
# include <cstdint>

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

struct GeometryData
{
	GeometryType	type;
	Transform		transform;		// Unused for Triangle
	AABB			boundingBox;	// Precomputed AABB in local space for broad-phase culling

	double			thickness;		// Torus:       minorRadius / majorRadius
	double			waist;			// Hyperboloid: waist parameter
	double			sharpness;		// Kiss:        sharpness parameter

	Point3			v0;				// Triangle:    vertex 0
	Point3			v1;				// Triangle:    vertex 1
	Point3			v2;				// Triangle:    vertex 2
	Vector3			triNormal;		// Triangle:    face normal
};

Ray				getLocalRay(const Transform &transform, const Ray &ray);

GeometryData	makeSphere      (const Point3 &center, double radius);
GeometryData	makePlane       (const Point3 &point, const Vector3 &normal);
GeometryData	makeCylinder    (double radius, double height, const Point3 &center, const Vector3 &normal);
GeometryData	makeCone        (double radius, double height, const Point3 &center, const Vector3 &normal);
GeometryData	makeHyperboloid (double waist, double radius, double height, const Point3 &center, const Vector3 &normal);
GeometryData	makeTorus       (double majorRadius, double minorRadius, const Point3 &center, const Vector3 &normal);
GeometryData	makeKiss        (double sharpness, const Point3 &center, const Vector3 &normal);
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

