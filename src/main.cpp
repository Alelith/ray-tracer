#include "Image.hpp"
#include "Vec3.hpp"
#include "RNG.hpp"
#include "Film.hpp"
#include "Sampling.hpp"

#include <cstdio>
// Mini experimento Monte Carlo: estimar pi lanzando puntos al azar en un cuadrado.
static void estimatePi() {
	uint32_t rng = makeSeed(0, 0);
	const int n = 1'000'000;
	int inside = 0;
	for (int i = 0; i < n; ++i) {
		float x = rngNext(rng), y = rngNext(rng);
		if (x * x + y * y < 1.0f) ++inside;
	}
	std::printf("pi ~= %.4f (con %d muestras)\n", 4.0 * inside / n, n);
}

int main() {
	estimatePi();

	const int width = 400;
	const int height = 225;
	Film film(width, height);

	// Pintamos un degradado con Vec3 y le sumamos ruido aleatorio por píxel
	// para comprobar que el RNG por píxel funciona (cada píxel tiene su semilla).
	const int samples = 16;
	for (int s = 0; s < samples; ++s) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				uint32_t pixel = uint32_t(y * width + x);
				uint32_t rng = makeSeed(pixel, uint32_t(s));

				float u = (float(x) + rngNext(rng)) / width;
				float v = (float(y) + rngNext(rng)) / height;
				Color c = lerp(Color(1.0f, 0.4f, 0.1f), Color(0.1f, 0.3f, 1.0f), u) * (1.0f - 0.6f * v);
				film.add(pixel, c);
			}
		}
	}

	Image img = film.resolve(samples);
	writePPM("out.ppm", img);
	std::printf("Escrita out.png\n");
	return 0;
}

