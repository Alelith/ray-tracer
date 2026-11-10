#include <chrono>
#include <cstdio>

#include "Camera.hpp"
#include "Film.hpp"
#include "Scene.hpp"
#include "WavefrontRenderer.hpp"

int main() {
	const int width = 400;
	const int height = 225;
	const int samplesPerPixel = 100;
	const int maxDepth = 20;
	const float aspect = float(width) / float(height);

	Scene scene;
	uint32_t ground = scene.addMaterial(Material::lambertian(Color(0.8f, 0.8f, 0.0f)));
	uint32_t center = scene.addMaterial(Material::lambertian(Color(0.1f, 0.2f, 0.5f)));
	uint32_t glass = scene.addMaterial(Material::dielectric(Color(0.8f, 0.6f, 0.2f), 1.5f));
	uint32_t right = scene.addMaterial(Material::metal(Color(0.8f, 0.6f, 0.2f), 0.05f));

	scene.addSphere(Point3(0, -100.5f, -1), 100.0f, ground);
	scene.addSphere(Point3(0, 0, -1.2f), 0.5f, center);
	scene.addSphere(Point3(-1.0f, 0, -1), 0.5f, glass);
	scene.addSphere(Point3(-1.0f, 0, -1), -0.4f, glass);  // radio negativo = burbuja hueca: la normal apunta hacia dentro
	scene.addSphere(Point3(1.0f, 0, -1), 0.5f, right);

	Camera camera(Point3(0, 0.4f, 1.2f), Point3(0, 0, -1), Vec3(0, 1, 0), 45.0f, aspect);

	Film film(width, height);
	WavefrontRenderer renderer(scene, camera, width, height, maxDepth);

	auto t0 = std::chrono::steady_clock::now();
	for (int s = 0; s < samplesPerPixel; ++s) {
		renderer.renderPass(uint32_t(s), film, /*printStats=*/s == 0);
	}
	double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

	writePNG("out.png", film.resolve(samplesPerPixel));
    std::printf("Escrita out.png (%d spp en %.2f s)\n", samplesPerPixel, secs);
	return 0;
}
