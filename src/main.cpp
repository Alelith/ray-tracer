#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Camera.hpp"
#include "Film.hpp"
#include "Megakernel.hpp"
#include "Mesh.hpp"
#include "Parallel.hpp"
#include "Scene.hpp"
#include "WavefrontRenderer.hpp"

int main(int argc, char** argv) {
	bool useBVH = true;
	bool megakernel = false;
	int samplesPerPixel = 100;
	std::string meshPath = "assets/dragon.obj";
	for (int i = 1; i < argc; ++i) {
		if (!std::strcmp(argv[i], "--help")) {
			std::printf("Uso: %s [opciones]\n", argv[0]);
			std::printf("Opciones:\n");
			std::printf("  --no-bvh          Desactiva la construcción del BVH\n");
			std::printf("  --megakernel      Usa el trazado de rayos en un solo kernel\n");
			std::printf("  --threads <N>     Número de hilos a usar (por defecto: hardware_concurrency)\n");
			std::printf("  --spp <N>         Número de muestras por píxel (por defecto: 100)\n");
			std::printf("  --mesh <path>     Ruta al archivo OBJ de la malla (por defecto: assets/dragon.obj)\n");
			return 0;
		}
		if (!std::strcmp(argv[i], "--no-bvh")) useBVH = false;
		if (!std::strcmp(argv[i], "--megakernel")) megakernel = true;
		if (!std::strcmp(argv[i], "--threads") && i + 1 < argc) ThreadPool::instance().setThreadCount(std::atoi(argv[++i]));
		if (!std::strcmp(argv[i], "--spp") && i + 1 < argc) samplesPerPixel = std::atoi(argv[++i]);
		if (!std::strcmp(argv[i], "--mesh") && i + 1 < argc) meshPath = argv[++i];
	}

	const int width = 400;
	const int height = 225;
	const int maxDepth = 20;
	const float aspect = float(width) / float(height);

	Scene scene;
	scene.skyHorizon = Color(0.35f, 0.4f, 0.5f);
	scene.skyZenith = Color(0.08f, 0.15f, 0.35f);

	uint32_t ground = scene.addMaterial(Material::lambertian(Color(0.55f, 0.55f, 0.55f)));
	uint32_t red = scene.addMaterial(Material::lambertian(Color(0.7f, 0.2f, 0.2f)));
	uint32_t blue = scene.addMaterial(Material::lambertian(Color(0.2f, 0.35f, 0.75f)));
	uint32_t gold = scene.addMaterial(Material::metal(Color(0.9f, 0.65f, 0.25f), 0.03f));
	uint32_t lamp = scene.addMaterial(Material::emissive(Color(14.0f, 12.5f, 11.0f)));

	scene.addSphere(Point3(0, -1000.0f, 0), 1000.0f, ground);
	scene.addSphere(Point3(2.1f, 0.5f, 0.8f), 0.5f, red);
	scene.addSphere(Point3(-2.2f, 0.6f, 0.9f), 0.6f, blue);
	scene.addSphere(Point3(-4.0f, 5.0f, 3.5f), 1.2f, lamp);

	Mesh mesh;
	if (!loadOBJ(meshPath, mesh)) {
		std::fprintf(stderr, "No puedo cargar %s\n", meshPath.c_str());
		return 1;
	}
	// La malla cabe en [-1,1]^3 (es más ancha que alta). La escalamos y la apoyamos en el suelo.
	scene.addMesh(mesh, gold, 1.5f, Vec3(0, 0.65f, 0));

	auto tb = std::chrono::steady_clock::now();
	scene.finalize(useBVH);
	double buildSecs = std::chrono::duration<double>(std::chrono::steady_clock::now() - tb).count();
	std::printf("%zu primitivas, %zu nodos BVH, construcción %.3f s (BVH %s)\n", scene.primitiveCount(),
				scene.bvhNodeCount(), buildSecs, useBVH ? "activado" : "desactivado");

	Camera camera(Point3(0, 2.2f, 6.0f), Point3(0, 0.9f, 0), Vec3(0, 1, 0), 38.0f, aspect);

	Film film(width, height);
	WavefrontRenderer renderer(scene, camera, width, height, maxDepth);

	auto t0 = std::chrono::steady_clock::now();
	for (int s = 0; s < samplesPerPixel; ++s) {
		if (megakernel)
			renderMegakernelPass(scene, camera, width, height, maxDepth, true, s, film);
		else
			renderer.renderPass(s, film, s == 0);
	}
	double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

	writePNG("out.png", film.resolve(samplesPerPixel));
	std::printf("Escrita out.png (%s, %u hilos, %d spp en %.2f s)\n", megakernel ? "megakernel" : "wavefront", ThreadPool::instance().threadCount(), samplesPerPixel, secs);
	return 0;
}

