#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include "Random.hpp"
# include "Camera.hpp"
# include "HitRecord.hpp"
# include <cstdint>

enum class MaterialType : uint8_t
{
	Lambertian,
	Metallic,
	Dielectric,
	Emmisive
};

enum class TextureType : uint8_t
{
	SolidColor
};

struct TextureData
{
	TextureType	type;
	Color		albedo;
};

struct MaterialData
{
	MaterialType	type;
	TextureData		albedo;
	double			param;
};

Color	textureGetColor(const TextureData &data, double u, double v, const Point3 &point);

bool	materialScatter(const MaterialData &data, const Ray &rayIn, const HitRecord &rec, Color &attenuation, Ray &scattered, RNG &rng);
Color	materialEmmited(const MaterialData &data, double u, double v, const Point3 &p);

#endif /* MATERIAL_HPP */
