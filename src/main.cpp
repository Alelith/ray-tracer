#include <cstdio>

#include "Camera.hpp"
#include "Film.hpp"
#include "PathQueue.hpp"
#include "Scene.hpp"
#include "Stages.hpp"

int main() {
	const int width = 1920;
	const int height = 1080;
	const float aspect = float(width) / float(height);

	Scene scene;
	scene.addSphere(Point3(0, 0, -1), 0.5f);
	scene.addSphere(Point3(0.5, 1, -1.5), 0.5f);
	scene.addSphere(Point3(0, -100.5f, -1), 100.0f);  // "suelo": una esfera gigante

	Camera camera(Point3(0, 0, 0), Point3(0, 0, -1), Vec3(0, 1, 0), 120.0f, aspect);

	Film film(width, height);
	const uint32_t numPaths = uint32_t(width * height);

	PathQueue paths;
	paths.allocate(numPaths);
	IndexQueue missQueue, hitQueue;
	missQueue.allocate(numPaths);
	hitQueue.allocate(numPaths);

	stageGenerateCamera(camera, width, height, paths);
	stageIntersect(scene, paths, missQueue, hitQueue);
	stageMissSky(paths, missQueue, film);
	stageShadeNormals(scene, paths, hitQueue, film);

	writePPM("out.ppm", film.resolve(1));
	std::printf("Escrita out.png (%u aciertos, %u fallos)\n", hitQueue.size(), missQueue.size());
	return 0;
}
