#include <cstdio>

#include "Camera.hpp"
#include "Film.hpp"
#include "PathQueue.hpp"
#include "Stages.hpp"

int main() {
	const int width = 400;
	const int height = 225;
	const float aspect = float(width) / float(height);

	Camera camera(Point3(0, 0, 0), Point3(0, 0, -1), Vec3(0, 1, 0), 120.0f, aspect);

	Film film(width, height);
	PathQueue queue;
	queue.allocate(uint32_t(width * height));

	// Un "frame" wavefront mínimo: generar -> (intersectar) -> sombrear los fallos.
	stageGenerateCamera(camera, width, height, queue);
	stageMissSky(queue, film);

	writePPM("out120fov.ppm", film.resolve(1));
	std::printf("Escrita out.png\n");
	return 0;
}
