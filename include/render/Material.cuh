#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Random.cuh"
# include "Ray.cuh"
# include "HitRecord.cuh"
# include "Matrix4x4.cuh"
# include "Texture.cuh"

namespace render
{

	using namespace maths;
	using namespace geometry;

	struct Material
	{
		Color	albedo;
		cudaTextureObject_t	albedoTexture;
		float	roughness;
		cudaTextureObject_t	roughnessTexture;
		float	metallic;
		cudaTextureObject_t	metallicTexture;
		float	transmission;
		cudaTextureObject_t	transmissionTexture;
		float	ior;
		Color	emmision;
		cudaTextureObject_t	emmisionTexture;

		float	emmisionStrength;

		Material() : albedo(1.0f, 1.0f, 1.0f), roughness(0.0f), metallic(0.0f), transmission(0.0f), ior(1.0f), emmision(0.0f, 0.0f, 0.0f), emmisionStrength(0.0f) {}
		Material(const Color &albedo, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, cudaTextureObject_t albedoTexture = 0, cudaTextureObject_t roughnessTexture = 0, cudaTextureObject_t metallicTexture = 0, cudaTextureObject_t transmissionTexture = 0, cudaTextureObject_t emmisionTexture = 0) : albedo(albedo), roughness(roughness), metallic(metallic), transmission(transmission), ior(ior), emmision(emmision), emmisionStrength(emmisionStrength), albedoTexture(albedoTexture), roughnessTexture(roughnessTexture), metallicTexture(metallicTexture), transmissionTexture(transmissionTexture), emmisionTexture(emmisionTexture) {}
	};

	struct MaterialsList
	{
		Material	*materials = nullptr;
		size_t		count = 0;
	};

	using Materials = MaterialsList;

	struct ScatterResult
	{
		bool	scatter;
		Ray		scatteredRay;
		Color	attenuation;
		Color	emmision;
	};

	void	addMaterial(const Color &color, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, const char texturesPaths[5][255], Materials &materials);

	__device__ ScatterResult	calculateBounce(const Material &mat, const Ray &ray, const HitRecord &record, Random &random);

} // namespace render

#endif /* MATERIAL_HPP */
