#include "Material.hpp"

void	addMaterial(const Color &color, float metallic, float roughness, float transmission, float ior, Materials &materials)
{
	materials.albedos.push_back(color);
	materials.metallic.push_back(metallic);
	materials.roughness.push_back(roughness);
	materials.transmission.push_back(transmission);
	materials.ior.push_back(ior);
}

inline Vector3	calculateDiffuse(const Vector3 &normal)
{
	Vector3	scatterDir = normal + Random::global().randomUnitVector3();
	if (scatterDir.isNearZero())
		scatterDir = normal;
	return scatterDir.normalized();
}

inline Vector3	calculateMetallic(const Vector3 &normal, const Vector3 &viewDir, float roughness)
{
	Vector3 reflected = reflect(viewDir, normal);
	return (reflected + roughness * Random::global().randomUnitVector3()).normalized();
}

inline Vector3	calculateGlass(const Vector3 &normal, const Vector3 &viewDir, float refractiveIndex, bool frontFace)
{
	float	refractionRatio = frontFace ? 1.0 / refractiveIndex : refractiveIndex;
	
	float	cosTheta = fmin(dot(-viewDir, normal), 1.0);
	float	sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	bool	cannotRefract = refractionRatio * sinTheta > 1.0;

	if (cannotRefract || reflectance(cosTheta, refractionRatio) > Random::global().randomfloat(0, 1))
		return reflect(viewDir, normal);
	return refract(viewDir, normal, refractionRatio);
}

ScatterResult	calculateBounce(const Ray &rayIn, const HitRecord &record, const Materials &materials)
{
	ScatterResult	res;

	Vector3	albedo = materials.albedos[record.geomIdx];
	float	metallic = materials.metallic[record.geomIdx];
	float	roughness = materials.roughness[record.geomIdx];
	float	transmission = materials.transmission[record.geomIdx];
	float	ior = materials.ior[record.geomIdx];

	float	chance = Random::global().randomfloat(0, 1);

	Vector3	newDir;

	if (chance < transmission)
		newDir = calculateGlass(record.normal, rayIn.getDirection(), ior, record.frontFace);
	else if (chance < transmission + metallic)
		newDir = calculateMetallic(record.normal, rayIn.getDirection(), roughness);
	else
		newDir = calculateDiffuse(record.normal);
	res.scatter = true;
	res.attenuation = albedo;
	res.scatteredRay = Ray(record.point + record.normal * 0.001, newDir);
	return res;
}
