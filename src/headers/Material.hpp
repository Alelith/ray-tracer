#ifndef MATERIAL_HPP
# define MATERIAL_HPP

# include <cstdint>
# include "Vec3.hpp"

enum class MaterialType : uint32_t {
	Lambertian = 0,
	Metal,
	Dielectric,
	Count
};

constexpr uint32_t	kMaterialCount = static_cast<uint32_t>(MaterialType::Count);

struct Material {
	MaterialType	type = MaterialType::Lambertian;
	Color			albedo{0.5f, 0.5f, 0.5f};
	float			fuzz = 0.0f;
	float			ior = 1.5f;

	static Material	lambertian(Color albedo);
	static Material	metal(Color albedo, float fuzz);
	static Material	dielectric(Color albedo, float ior);
};

# include "detail/Material.ipp"

#endif // MATERIAL_HPP
