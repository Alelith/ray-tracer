#include <chrono>
#include <cstdio>
#include <cstring>

#include "Camera.hpp"
#include "Film.hpp"
#include "Scene.hpp"
#include "WavefrontRenderer.hpp"

int main(int argc, char** argv) {
	bool useNEE = true;
	int samplesPerPixel = 1000;
	for (int i = 1; i < argc; ++i) {
		if (std::strcmp(argv[i], "--no-nee") == 0) useNEE = false;
		if (std::strcmp(argv[i], "--spp") == 0 && i + 1 < argc) samplesPerPixel = std::atoi(argv[++i]);
	}

	const int width = 400;
	const int height = 225;
	const int maxDepth = 20;
	const float aspect = float(width) / float(height);

	Scene scene;
	scene.skyHorizon = Color(0.02f, 0.02f, 0.03f);  // noche: casi toda la luz viene de las lámparas
	scene.skyZenith = Color(0.005f, 0.01f, 0.03f);

	uint32_t ground = scene.addMaterial(Material::lambertian(Color(0.6f, 0.6f, 0.6f)));
	uint32_t red = scene.addMaterial(Material::lambertian(Color(0.7f, 0.2f, 0.2f)));
	uint32_t glass = scene.addMaterial(Material::dielectric(Color(1.0f, 1.0f, 1.0f), 1.5f));
	uint32_t gold = scene.addMaterial(Material::metal(Color(0.8f, 0.6f, 0.2f), 0.05f));
	uint32_t lampBig = scene.addMaterial(Material::emissive(Color(4.0f, 3.6f, 3.2f)));
	uint32_t lampSmall = scene.addMaterial(Material::emissive(Color(60.0f, 90.0f, 140.0f)));

	scene.addSphere(Point3(0, -100.5f, -1), 100.0f, ground);
	scene.addSphere(Point3(0, 0, -1.2f), 0.5f, red);
	scene.addSphere(Point3(-1.0f, 0, -1), 0.5f, glass);
	scene.addSphere(Point3(1.0f, 0, -1), 0.5f, gold);
	scene.addSphere(Point3(-1.5f, 1.8f, -0.5f), 0.7f, lampBig);
	scene.addSphere(Point3(0.6f, 0.0f, -0.2f), 0.1f, lampSmall);

	Camera camera(Point3(0, 0.4f, 1.2f), Point3(0, 0, -1), Vec3(0, 1, 0), 45.0f, aspect);

	Film film(width, height);
	WavefrontRenderer renderer(scene, camera, width, height, maxDepth, useNEE);

	auto t0 = std::chrono::steady_clock::now();
	for (int s = 0; s < samplesPerPixel; ++s) {
		renderer.renderPass(uint32_t(s), film, /*printStats=*/s == 0);
	}
	double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

	writePNG("out.png", film.resolve(samplesPerPixel));
    std::printf("Escrita out.png (%d spp en %.2f s)\n", samplesPerPixel, secs);
	return 0;
}
