#ifndef GEOMETRY_HPP
# define GEOMETRY_HPP

# include "Solver.cuh"
# include "AABB.cuh"
# include "HitRecord.cuh"
# include "Limits.cuh"
# include <cstdint>
# include <vector>

namespace geometry
{

	using namespace maths;

	enum GeomType
	{
		SPHERE,      
		PLANE,       
		CYLINDER,    
		CONE,        
		HYPERBOLOID, 
		TORUS,       
		KISS,        
		TRIANGLE,    
		QUAD,        
		NONE         
	};

	struct Primitive
	{
		GeomType	type;
		AABB		boundingBox;
		int			geomIdx;

		Matrix4x4	inverseTransform;
		float		param;

		union GeometryData
		{
			struct
			{
				Point3	v0, v1, v2;
				Vector3	normal;
			}	tris;
			struct
			{
				Point3	Q;
				Vector3	u, v, w;
				Vector3	normal;
				float	D;
			}	quad;
			GeometryData() {}
		} data;
		Primitive() : type(NONE), geomIdx(-1), param(0), data() {}	
	};

	struct WorldGeometries
	{
		Primitive	*primitives = nullptr;
		size_t		count;

		WorldGeometries		cudaCopy() const;
		void				cudaFreeWorld();
	};

	using PrimitivesList = std::vector<Primitive>;

	RT_CALLABLE Ray	getLocalRay(const Matrix4x4 &inverse, const Ray &ray);

	void	makeSphere(const Point3 &center, float radius, PrimitivesList &primitives);
	void	makeCylinder(float radius, float height, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives);
	void	makeCone(float radius, float height, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives);
	void	makeTorus(float majorRadius, float minorRadius, const Point3 &center, const Vector3 &normal, PrimitivesList &primitives);
	void	makeTriangle(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, PrimitivesList &primitives);
	void	makeQuad(const Point3 &Q, const Vector3 &u, const Vector3 &v, PrimitivesList &primitives);

	RT_CALLABLE bool	sphereHits(const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
	RT_CALLABLE bool	cylinderHits(const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
	RT_CALLABLE bool	coneHits(const Matrix4x4 &inverseTransform, const Ray &ray, const Limits &limits, HitRecord &record);
	RT_CALLABLE bool	torusHits(const Matrix4x4 &inverseTransform, float param, const Ray &ray, const Limits &limits, HitRecord &record);
	RT_CALLABLE bool	triangleHits(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Vector3 &normal, const Ray &ray, const Limits &limits, HitRecord &record);
	RT_CALLABLE bool	quadHits(const Point3 &Q, const Vector3 &u, const Vector3 &v, const Vector3 &w, const Vector3 &normal, float D, const Ray &ray, const Limits &limits, HitRecord &record);

} // namespace geometry

#endif /* GEOMETRY_HPP */
