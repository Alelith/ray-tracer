#include "Material.hpp"

MaterialData	makeLambertian(const Color &albedo)
{
	MaterialData m{};
	m.type = Material::Lambertian;
	m.albedo = albedo;
	return m;
}

MaterialData	makeMetallic(const Color &albedo, double roughness)
{
	MaterialData m{};
	m.type = Material::Metallic;
	m.albedo = albedo;
	m.roughness = roughness;
	return m;
}

MaterialData	makeDielectric(const Color &albedo, double refractiveIndex, double opacity)
{
	MaterialData m{};
	m.type = Material::Dielectric;
	m.albedo = albedo;
	m.refractiveIndex = refractiveIndex;
	m.opacity = opacity;
	return m;
}

bool	scatterLambertian(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered)
{
	(void)rIn; // Unused parameter
	Random &random = Random::global();
	Vector3 scatterDirection = record.normal + random.randomUnitVector3();
	if (scatterDirection.isNearZero())
		scatterDirection = record.normal;
	scattered = Ray(record.point, scatterDirection);
	attenuation = material.albedo;
	return true;
}

bool	scatterMetallic(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered)
{
	Vector3 reflected = reflect(rIn.getDirection().normalized(), record.normal);
	Random &random = Random::global();
	Vector3 fuzz = material.roughness * random.randomInUnitSphere();
	scattered = Ray(record.point, reflected + fuzz);
	attenuation = material.albedo;
	return dot(scattered.getDirection(), record.normal) > 0;
}

bool	scatterDielectric(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered)
{
	Vector3	unitDirection = rIn.getDirection().normalized();

	double	ri;
	if (record.frontFace)
		ri = 1.0 / material.refractiveIndex;
	else
		ri = material.refractiveIndex;
	
	double	cosTheta = fmin(dot(-unitDirection, record.normal), 1.0);
	double	sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	double	r0 = (1.0 - ri) / (1.0 + ri);
	r0 = r0 * r0;
	r0 = r0 + (0.5 - r0) * material.opacity;

	double	fresnel = r0 + (1.0 - r0) * pow(1.0 - cosTheta, 2.0 + 3.0 * (1.0 - material.opacity));

	Random	&random = Random::global();
	bool	willReflect = ri * sinTheta > 1.0 || fresnel > random.randomDouble(0.0, 1.0);
	if (!willReflect && random.randomDouble(0.0, 1.0) < material.opacity)
		return scatterLambertian(rIn, material, record, attenuation, scattered);
	
	if (willReflect && record.frontFace)
		attenuation = Color::One - material.opacity * (Color::One - material.albedo);
	else if (!record.frontFace)
	{
		Color baseAttenuation = -record.t * (Color::One - material.albedo);
		attenuation = Color(exp(baseAttenuation[0]), exp(baseAttenuation[1]), exp(baseAttenuation[2]));
	}
	else
		attenuation = Color::One;
	
	Vector3	direction;
	if (willReflect)
		direction = reflect(unitDirection, record.normal);
	else
		direction = refract(unitDirection, record.normal, ri);
	scattered = Ray(record.point, direction);

	return true;
}

bool	scatter(const Ray &rIn, const MaterialData &material, const HitRecord &record, Color &attenuation, Ray &scattered)
{
	switch (material.type)
	{
		case Material::Lambertian:
			return scatterLambertian(rIn, material, record, attenuation, scattered);
		case Material::Metallic:
			return scatterMetallic(rIn, material, record, attenuation, scattered);
		case Material::Dielectric:
			return scatterDielectric(rIn, material, record, attenuation, scattered);
		default:
			return false;
	}
}
