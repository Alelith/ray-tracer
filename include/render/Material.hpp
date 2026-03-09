#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Random.hpp"
# include "Ray.hpp"
# include "HitRecord.hpp"
# include <vector>

using std::vector;

struct Materials
{
	vector<Color>	albedos;
	vector<float>	metallic;
	vector<float>	roughness;
	vector<float>	transmission;
	vector<float>	ior;
};

struct ScatterResult
{
	bool	scatter;	
	Color	attenuation;
	Ray		scatteredRay;
};

void			addMaterial       (const Color &color, float metallic, float roughness, float transmission, float ior, Materials &materials);

inline Vector3	calculateDiffuse  (const Vector3 &normal);
inline Vector3	calculateMetallic (const Vector3 &normal, const Vector3 &viewDir, float roughness);
inline Vector3	calculateGlass    (const Vector3 &normal, const Vector3 &viewDir, float refractiveIndex, bool frontFace);

ScatterResult	calculateBounce   (const Ray &rayIn, const HitRecord &record, const Materials &materials);

#endif /* MATERIAL_HPP */
