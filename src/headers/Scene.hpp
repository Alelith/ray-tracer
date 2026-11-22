#ifndef SCENE_HPP
# define SCENE_HPP

# include <cstdint>
# include <vector>
# include "Ray.hpp"
# include "Material.hpp"
# include "Mesh.hpp"
# include "BVH.hpp"

class Scene {
	public:
		Color skyHorizon{0.5f, 0.7f, 1.0f};
		Color skyZenith{1.0f, 1.0f, 1.0f};

		inline uint32_t						addMaterial(const Material& m);
		inline void							addSphere(Point3 center, float radius, uint32_t material);
		inline void							addTriangle(Point3 a, Point3 b, Point3 c, uint32_t material);
		inline void							addMesh(const Mesh& mesh, uint32_t material, float scale, Vec3 translate);
		inline void							finalize(bool useBVH = true);
		inline Color						sky(const Vec3& dir) const;
		inline const Material&				materialOf(uint32_t prim) const;
		inline const Primitive&				primitive(uint32_t prim) const;
		inline const std::vector<uint32_t>&	getLights() const;
		inline size_t						primitiveCount() const;
		inline size_t						bvhNodeCount() const;
		inline bool							closestHit(const Ray& ray, float tMin, float tMax, float& tHit, uint32_t& primHit) const;
		inline bool							occluded(const Ray& ray, float tMin, float tMax) const;
		inline Vec3							outwardNormal(uint32_t prim, const Point3& p) const;
	
	private:
		std::vector<Primitive>	prims;
		std::vector<Material>	materials;
		std::vector<uint32_t>	lights;
		
		BVH		bvh;
		bool	useBVH = true;
};

# include "detail/Scene.ipp"

#endif // SCENE_HPP
