#ifndef SCENES_HPP
# define SCENES_HPP

# include "Objects/Camera.hpp"
# include "Objects/Mesh.hpp"
# include "Maths/RNG.hpp"
# include "Objects/Scene.hpp"

// Escena "portada": un suelo, un nudo dorado, unas cuantas esferas grandes y un
// campo de esferitas aleatorias de los tres materiales. Una lámpara esférica grande
// fuera de cuadro ilumina todo (el resto de la luz viene del cielo).
inline bool buildCoverScene(Scene& scene, const std::string& meshPath, float aspect, Camera& camera) {
	scene.skyHorizon = Color(0.55f, 0.65f, 0.85f);
	scene.skyZenith = Color(0.12f, 0.22f, 0.5f);

	uint32_t ground = scene.addMaterial(Material::lambertian(Color(0.5f, 0.5f, 0.52f)));
	uint32_t gold = scene.addMaterial(Material::metal(Color(0.95f, 0.7f, 0.25f), 0.02f));
	uint32_t glass = scene.addMaterial(Material::dielectric(Color(1, 1, 1), 1.5f));
	uint32_t red = scene.addMaterial(Material::lambertian(Color(0.75f, 0.15f, 0.15f)));
	uint32_t mirror = scene.addMaterial(Material::metal(Color(0.85f, 0.85f, 0.9f), 0.0f));
	uint32_t lamp = scene.addMaterial(Material::emissive(Color(14.0f, 13.0f, 11.5f)));

	scene.addSphere(Point3(0, -1000.0f, 0), 1000.0f, ground);
	scene.addSphere(Point3(-3.4f, 0.9f, 0.6f), 0.9f, glass);
	scene.addSphere(Point3(3.4f, 0.9f, 0.4f), 0.9f, red);
	scene.addSphere(Point3(0.2f, 0.6f, 2.6f), 0.6f, mirror);
	scene.addSphere(Point3(-6.0f, 7.0f, 5.0f), 1.8f, lamp);

	// Campo de esferas pequeñas con semilla fija: la escena es siempre la misma.
	uint32_t seed = 1234;
	for (int a = -7; a <= 7; ++a) {
		for (int b = -4; b <= 5; ++b) {
			float x = float(a) + 0.8f * rngNext(seed);
			float z = float(b) + 0.8f * rngNext(seed);
			float choose = rngNext(seed);
			float r = 0.18f + 0.1f * rngNext(seed);
			// Dejamos libre la zona central (nudo y esferas grandes).
			if (std::fabs(x) < 4.6f && z > -1.8f && z < 3.4f) continue;

			if (choose < 0.7f) {
				Color c(rngNext(seed) * rngNext(seed), rngNext(seed) * rngNext(seed), rngNext(seed) * rngNext(seed));
				scene.addSphere(Point3(x, r, z), r, scene.addMaterial(Material::lambertian(c)));
			} else if (choose < 0.9f) {
				Color c(0.5f + 0.5f * rngNext(seed), 0.5f + 0.5f * rngNext(seed), 0.5f + 0.5f * rngNext(seed));
				scene.addSphere(Point3(x, r, z), r, scene.addMaterial(Material::metal(c, 0.4f * rngNext(seed))));
			} else {
				scene.addSphere(Point3(x, r, z), r, glass);
			}
		}
	}

	Mesh mesh;
	if (!loadOBJ(meshPath, mesh)) return false;
	scene.addMesh(mesh, gold, 1.5f, Vec3(0, 0.65f, 0.4f));

	camera = Camera(Point3(0, 2.2f, 9.5f), Point3(0, 0.3f, 0), Vec3(0, 1, 0), 36.0f, aspect, 0.0f, 1.0f);
	return true;
}

#endif // SCENES_HPP
