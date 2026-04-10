#include "Material.cuh"

void	addMaterial(const Color &color, float roughness, float metallic, float transmission, float ior, const Color &emmision, float emmisionStrength, const char texturesPaths[5][255], Materials &materials)
{
	if (texturesPaths)
	{
		cudaTextureObject_t albedoTexture = 0;
		if (texturesPaths[0][0] != '\0')
			albedoTexture = Texture(texturesPaths[0]).image;
		cudaTextureObject_t roughnessTexture = 0;
		if (texturesPaths[1][0] != '\0')
			roughnessTexture = Texture(texturesPaths[1]).image;
		cudaTextureObject_t metallicTexture = 0;
		if (texturesPaths[2][0] != '\0')
			metallicTexture = Texture(texturesPaths[2]).image;
		cudaTextureObject_t transmissionTexture = 0;
		if (texturesPaths[3][0] != '\0')
			transmissionTexture = Texture(texturesPaths[3]).image;
		cudaTextureObject_t emmisionTexture = 0;
		if (texturesPaths[4][0] != '\0')
			emmisionTexture = Texture(texturesPaths[4]).image;

		Material newMat(color, roughness, metallic, transmission, ior, emmision, emmisionStrength, albedoTexture, roughnessTexture, metallicTexture, transmissionTexture, emmisionTexture);
		Material *newMaterials = new Material[materials.count + 1];

		for (size_t i = 0; i < materials.count; ++i)
			newMaterials[i] = materials.materials[i];

		newMaterials[materials.count] = newMat;

		delete[] materials.materials;
		materials.materials = newMaterials;
		materials.count++;
	}
	else
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
}

__device__ ScatterResult	calculateBounce(const Material &mat, const Ray &ray, const HitRecord &record, Random &random)
{
	ScatterResult	result;

	float4 emmisionFromTexture = make_float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (mat.emmisionTexture)
		emmisionFromTexture = tex2D<float4>(mat.emmisionTexture, record.u, record.v);
	Color emmisionColor = Color(emmisionFromTexture.x, emmisionFromTexture.y, emmisionFromTexture.z) * mat.emmision;
	result.scatter = emmisionColor.isNearZero();

	float4 albedoFromTexture;
	if (mat.albedoTexture)
		albedoFromTexture = tex2D<float4>(mat.albedoTexture, record.u, record.v);
	else
		albedoFromTexture = make_float4(1.0, 1.0f, 1.0f, 1.0f);

	float roughnessFromTexture = 1.0f;
	if (mat.roughnessTexture)
		roughnessFromTexture = tex2D<float4>(mat.roughnessTexture, record.u, record.v).x;
	float finalRoughness = mat.roughness * roughnessFromTexture;

	float metallicFromTexture = 1.0f;
	if (mat.metallicTexture)
		metallicFromTexture = tex2D<float4>(mat.metallicTexture, record.u, record.v).x;
	float finalMetallic = mat.metallic * metallicFromTexture;

	float transmissionFromTexture = 1.0f;
	if (mat.transmissionTexture)
		transmissionFromTexture = tex2D<float4>(mat.transmissionTexture, record.u, record.v).x;
	float finalTransmission = mat.transmission * transmissionFromTexture;

	float randomVal = random.randomfloat();
	if (randomVal < finalMetallic)
	{
		Vector3 reflected = reflect(ray.getDirection().normalized(), record.normal);
		reflected += finalRoughness * random.randomInUnitSphere(); // Placeholder for random roughness

		result.scatteredRay = Ray(record.point, reflected);
		result.attenuation = Color(albedoFromTexture.x, albedoFromTexture.y, albedoFromTexture.z) * mat.albedo;
	}
	else
	{
		float	cosTheta = fminf(dot(-ray.getDirection().normalized(), record.normal), 1.0f);

		float	reflectProb = reflectance(cosTheta, mat.ior);
		randomVal = random.randomfloat();

		if (randomVal < reflectProb)
		{
			Vector3 reflected = reflect(ray.getDirection().normalized(), record.normal);
			reflected += finalRoughness * random.randomInUnitSphere(); // Placeholder for random roughness

			result.scatteredRay = Ray(record.point, reflected);
			result.attenuation = Color(1.0, 1.0, 1.0);
		}
		else
		{
			randomVal = random.randomfloat();
			if (randomVal < finalTransmission)
			{
				float etaiOverEtat = record.frontFace ? (1.0f / mat.ior) : mat.ior;

				Vector3 refractionDir = refract(ray.getDirection().normalized(), record.normal, etaiOverEtat);
				refractionDir += finalRoughness * random.randomInUnitSphere(); // Placeholder for random roughness

				result.scatteredRay = Ray(record.point, refractionDir);
				result.attenuation = Color(albedoFromTexture.x, albedoFromTexture.y, albedoFromTexture.z) * mat.albedo;
			}
			else
			{
				Vector3 scatterDirection = record.normal + random.randomUnitVector3(); // Placeholder for random scatter direction
				if (scatterDirection.isNearZero())
					scatterDirection = record.normal;

				result.scatteredRay = Ray(record.point, scatterDirection);
				result.attenuation = Color(albedoFromTexture.x, albedoFromTexture.y, albedoFromTexture.z) * mat.albedo;
			}
		}
	}
	
	result.emmision = emmisionColor * mat.emmisionStrength;
	return result;
}
