#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Random.hpp"
# include "Ray.hpp"
# include "HitRecord.hpp"

enum class Material
{
	Lambertian,
	Metallic,
	Dielectric
};

struct MaterialData
{
	Material	type;
	Color		albedo;
	float		roughness; // For Metallic: 0.0 (perfect mirror) to 1.0 (fully rough)
	float		refractiveIndex; // For Dielectric (not implemented yet)
	float		opacity; // For future use (e.g., transparent materials)
};

MaterialData	makeLambertian(const Color &albedo);
MaterialData	makeMetallic(const Color &albedo, float roughness);
MaterialData	makeDielectric(const Color &albedo, float refractiveIndex, float opacity = 1.0);

bool			scatterLambertian(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered);
bool			scatterMetallic(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered);
bool			scatterDielectric(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered);

bool			scatter(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered);

#endif /* MATERIAL_HPP */
