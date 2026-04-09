#include "Material.cuh"

void	addMaterial(const Color &color, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, Materials &materials)
{
	Material newMat(color, roughness, metallic, transmission, ior, emmision, emmisionStrength);
	Material *newMaterials = new Material[materials.count + 1];

	for (size_t i = 0; i < materials.count; ++i)
		newMaterials[i] = materials.materials[i];

	newMaterials[materials.count] = newMat;

	delete[] materials.materials;
	materials.materials = newMaterials;
	materials.count++;
}

RT_CALLABLE ScatterResult	calculateBounce(const Material &mat, const Ray &ray, const HitRecord &record, Random &random)
{
	ScatterResult	result;
	result.scatter = mat.emmision.isNearZero();

	float randomVal = random.randomfloat();
	if (randomVal < mat.metallic)
	{
		Vector3 reflected = reflect(ray.getDirection().normalized(), record.normal);
		reflected += mat.roughness * random.randomInUnitSphere(); // Placeholder for random roughness

		result.scatteredRay = Ray(record.point, reflected);
		result.attenuation = mat.albedo;
	}
	else
	{
		float	cosTheta = fminf(dot(-ray.getDirection().normalized(), record.normal), 1.0f);

		float	reflectProb = reflectance(cosTheta, mat.ior);
		randomVal = random.randomfloat();

		if (randomVal < reflectProb)
		{
			Vector3 reflected = reflect(ray.getDirection().normalized(), record.normal);
			reflected += mat.roughness * random.randomInUnitSphere(); // Placeholder for random roughness

			result.scatteredRay = Ray(record.point, reflected);
			result.attenuation = Color(1.0, 1.0, 1.0);
		}
		else
		{
			randomVal = random.randomfloat();
			if (randomVal < mat.transmission)
			{
				float etaiOverEtat = record.frontFace ? (1.0f / mat.ior) : mat.ior;

				Vector3 refractionDir = refract(ray.getDirection().normalized(), record.normal, etaiOverEtat);
				refractionDir += mat.roughness * random.randomInUnitSphere(); // Placeholder for random roughness

				result.scatteredRay = Ray(record.point, refractionDir);
				result.attenuation = mat.albedo;
			}
			else
			{
				Vector3 scatterDirection = record.normal + random.randomUnitVector3(); // Placeholder for random scatter direction
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
