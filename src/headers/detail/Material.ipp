Material Material::lambertian(Color albedo) {
	Material m;
	m.type = MaterialType::Lambertian;
	m.albedo = albedo;
	return m;
}
Material Material::metal(Color albedo, float fuzz) {
	Material m;
	m.type = MaterialType::Metal;
	m.albedo = albedo;
	m.fuzz = fuzz < 1.0f ? fuzz : 1.0f;
	return m;
}
Material Material::dielectric(Color albedo, float ior) {
	Material m;
	m.type = MaterialType::Dielectric;
	m.albedo = albedo;
	m.ior = ior;
	return m;
}
