/**
 * @file HitRecord.cuh
 * @brief Structure containing intersection information
 * 
 * @author Lilith Estévez Boeta
 * @date 2026-04-13
 */

#ifndef HITRECORD_HPP
# define HITRECORD_HPP

# include "Ray.cuh"

/**
 * @defgroup geometry_module Geometry Module
 * @brief Geometric primitives, intersections, and spatial partitioning structures
 * 
 * @details Covers primitives defined by equations, BVH acceleration,
 * axis-aligned bounding boxes (AABBs), and related geometric calculation tools.
 */

/**
 * @struct HitRecord
 * @brief Records intersection parameters upon surface hit
 * 
 * Contains information such as intersection point, distance factor t,
 * normal vectors, UV mapping coordinates, and flags identifying whether
 * the interaction occurred on the inner or outer face wrapper.
 * 
 * @ingroup geometry_module
 */
struct HitRecord
{
	Point3	point;     ///< The mapped 3D point of interception
	Vector3	normal;    ///< Extracted normal surface vector based on relative interacting orientation
	float	t;         ///< Extent intersection mapping along ray progression
	int		geomIdx;   ///< Storage reference identifier identifying exact geometry match bounded structures
	bool	frontFace; ///< Orientation tag defining interaction approach mapped outside orientation representation
	bool	hit;       ///< Collision status result representations flag bounds parameter mapped validity parameters representations

	float	u;  ///< Extracted mapping width normalized texture bounding relative parameters
	float	v;  ///< Extracted mapping length normalized texture bounds relative dimensions bounds coordinates representations

	RT_CALLABLE HitRecord() : t(INFINITY), geomIdx(-1), frontFace(false), hit(false), u(0), v(0) {}
	RT_CALLABLE HitRecord(bool hit, const Point3 &point, const Vector3 &normal, float t, bool frontFace)
		: point(point), normal(normal), t(t), geomIdx(-1), frontFace(frontFace), hit(hit), u(0), v(0) {}

	/**
	 * @brief Computes proper mapping normal parameter representations bounding faces geometry bounds representations calculations formats orientations structures limits
	 * 
	 * @param[in] ray Incoming incidence parameters combinations mappings equivalents coordinates directions
	 * @param[in] outwardNormal Normal geometries properties orientations surface parameters representations mapping
	 */
	inline RT_CALLABLE void	setFaceNormal(const Ray &ray, const Vector3 &outwardNormal)
	{
		frontFace = dot(ray.getDirection(), outwardNormal) < 0;
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

#endif /* HITRECORD_HPP */
