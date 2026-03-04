#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include <cstdint>
# include "Ray.hpp"
# include "Solver.hpp"
# include "Transform.hpp"
# include "HitRecord.hpp"
# include "Limits.hpp"

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

// Plain data struct — no vtable, no heap allocation, GPU-compatible.
// Only the fields labelled with a type are meaningful for that type;
// all unused fields are zero-initialised by the factory functions.
struct GeometryData
{
	GeometryType	type;
	Transform		transform;		// Unused for Triangle

	double			thickness;		// Torus:       minorRadius / majorRadius
	double			waist;			// Hyperboloid: waist parameter
	double			sharpness;		// Kiss:        sharpness parameter

	Point3			v0;				// Triangle:    vertex 0
	Point3			v1;				// Triangle:    vertex 1
	Point3			v2;				// Triangle:    vertex 2
	Vector3			triNormal;		// Triangle:    face normal
};

// ── Utility ────────────────────────────────────────────────────────────────────────────
Ray				getLocalRay(const Transform &transform, const Ray &ray);

// ── Factory functions ─ replace constructors ───────────────────────────────────────
GeometryData	makeSphere      (const Point3 &center, double radius);
GeometryData	makePlane       (const Point3 &point, const Vector3 &normal);
GeometryData	makeCylinder    (double radius, double height, const Point3 &center, const Vector3 &normal);
GeometryData	makeCone        (double radius, double height, const Point3 &center, const Vector3 &normal);
GeometryData	makeHyperboloid (double waist, double radius, double height, const Point3 &center, const Vector3 &normal);
GeometryData	makeTorus       (double majorRadius, double minorRadius, const Point3 &center, const Vector3 &normal);
GeometryData	makeKiss        (double sharpness, const Point3 &center, const Vector3 &normal);
GeometryData	makeTriangle    (const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal);

// ── Per-shape hit functions ─ called by geometryHit() ────────────────────────────
bool			sphereHit      (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			planeHit       (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			cylinderHit    (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			coneHit        (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			hyperboloidHit (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			torusHit       (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			kissHit        (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);
bool			triangleHit    (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);

// ── Main dispatch ─ replaces virtual hit() ───────────────────────────────────────────
bool			geometryHit    (const GeometryData &g, const Ray &ray, const Limits &limits, HitRecord &record);

#endif /* GEOMETRY_HPP */

