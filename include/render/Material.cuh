#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Random.cuh"
# include "Ray.cuh"
# include "HitRecord.cuh"
# include "Matrix4x4.cuh"

struct Material
{
	Color	albedo;

	float	roughness;
	float	metallic;
	float	transmission;
	float	ior;

	Color	emmision;
	float	emmisionStrength;

	Material() : albedo(1.0f, 1.0f, 1.0f), roughness(0.0f), metallic(0.0f), transmission(0.0f), ior(1.0f), emmision(0.0f, 0.0f, 0.0f), emmisionStrength(0.0f) {}
	Material(const Color &albedo, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength)
		: albedo(albedo), roughness(roughness), metallic(metallic), transmission(transmission), ior(ior), emmision(emmision), emmisionStrength(emmisionStrength) {}
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

void			addMaterial(const Color &color, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, Materials &materials);

RT_CALLABLE ScatterResult	calculateBounce(const Material &mat, const Ray &ray, const HitRecord &record, Random &random);

#endif /* MATERIAL_HPP */
