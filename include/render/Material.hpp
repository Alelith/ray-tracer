#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Random.hpp"
# include "Ray.hpp"
# include "HitRecord.hpp"
# include "Matrix4x4.hpp"
# include <vector>

using std::vector;

struct Material
{
	Color	albedo;

	float	roughness;
	float	metallic;
	float	transmission;
	float	ior;

	Color	emmision;
	float	emmisionStrength;

	Material(const Color &albedo, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength)
		: albedo(albedo), roughness(roughness), metallic(metallic), transmission(transmission), ior(ior), emmision(emmision), emmisionStrength(emmisionStrength) {}
};

using Materials = vector<Material>;

struct ScatterResult
{
	bool	scatter;	
	Ray		scatteredRay;
	Color	attenuation;
	Color	emmision;
};

void			addMaterial(const Color &color, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, Materials &materials);

ScatterResult	calculateBounce(const Material &mat, const Ray &ray, const HitRecord &record);

#endif /* MATERIAL_HPP */
