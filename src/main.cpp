#include <chrono>
#include <cstdio>

#include "Camera.hpp"
#include "Film.hpp"
#include "PathQueue.hpp"
#include "Scene.hpp"
#include "Stages.hpp"

int main() {
	const int width = 400;
	const int height = 225;
	const int samplesPerPixel = 256;
	const float aspect = float(width) / float(height);

	Scene scene;
	scene.addSphere(Point3(0, 0, -1), 0.5f);
	scene.addSphere(Point3(-1.1f, 0, -1.8f), 0.5f);
	scene.addSphere(Point3(1.1f, 0, -0.4f), 0.5f);
	scene.addSphere(Point3(0, -100.5f, -1), 100.0f);

	// Enfocamos la esfera central (a distancia 1) con una lente apreciable.
	Camera camera(Point3(0, 0.3f, 1), Point3(0, 0, -1), Vec3(0, 1, 0), 50.0f, aspect, 0.15f, 2.0f);

	Film film(width, height);
	const uint32_t numPaths = uint32_t(width * height);

	PathQueue paths;
	paths.allocate(numPaths);
	IndexQueue missQueue, hitQueue;
	missQueue.allocate(numPaths);
	hitQueue.allocate(numPaths);

	auto t0 = std::chrono::steady_clock::now();
	for (int s = 0; s < samplesPerPixel; ++s) {
		stageGenerateCamera(camera, width, height, uint32_t(s), paths);
		stageIntersect(scene, paths, missQueue, hitQueue);
		stageMissSky(paths, missQueue, film);
		stageShadeNormals(scene, paths, hitQueue, film);
	}
	double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

	writePPM("out_256spp.ppm", film.resolve(samplesPerPixel));
	std::printf("Escrita out.png (%d spp en %.2f s)\n", samplesPerPixel, secs);
	return 0;
}
