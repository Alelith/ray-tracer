#include "Material.hpp"

void	addMaterial(const Color &color, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, Materials &materials)
{
	materials.push_back(Material(color, roughness, metallic, transmission, ior, emmision, emmisionStrength));
}

ScatterResult	calculateBounce(const Material &mat, const Ray &ray, const HitRecord &record)
{
	ScatterResult	result;
	result.scatter = mat.emmision.isNearZero();

	float randomVal = Random::global().randomfloat(0, 1);
	if (randomVal < mat.metallic)
	{
		Vector3 reflected = reflect(ray.getDirection().normalized(), record.normal);
		reflected += mat.roughness * Random::global().randomInUnitSphere();

		result.scatteredRay = Ray(record.point, reflected);
		result.attenuation = mat.albedo;
	}
	else
	{
		float	cosTheta = std::min(dot(-ray.getDirection().normalized(), record.normal), 1.0f);

		float	reflectProb = reflectance(cosTheta, mat.ior);
		randomVal = Random::global().randomfloat(0, 1);

		if (randomVal < reflectProb)
		{
			Vector3 reflected = reflect(ray.getDirection().normalized(), record.normal);
			reflected += mat.roughness * Random::global().randomInUnitSphere();

			result.scatteredRay = Ray(record.point, reflected);
			result.attenuation = Color::White;
		}
		else
		{
			randomVal = Random::global().randomfloat(0, 1);
			if (randomVal < mat.transmission)
			{
				float etaiOverEtat = record.frontFace ? (1.0f / mat.ior) : mat.ior;

				Vector3 refractionDir = refract(ray.getDirection().normalized(), record.normal, etaiOverEtat);
				refractionDir += mat.roughness * Random::global().randomInUnitSphere();

				result.scatteredRay = Ray(record.point, refractionDir);
				result.attenuation = mat.albedo;
			}
			else
			{
				Vector3 scatterDirection = record.normal + Random::global().randomUnitVector3();
				if (scatterDirection.isNearZero())
					scatterDirection = record.normal;

				result.scatteredRay = Ray(record.point, scatterDirection);
				result.attenuation = mat.albedo;
			}
		}
	}

	result.emmision = mat.emmision * mat.emmisionStrength;
	return result;
}
