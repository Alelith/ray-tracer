#include "Image.hpp"
#include "Vec3.hpp"
#include "RNG.hpp"

int main() {
	const int width = 400;
	const int height = 225;

	Image image(width, height);

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			float r = static_cast<float>(x) / (width - 1);
			float g = static_cast<float>(y) / (height - 1);
			float b = 0.25f;

			image.set(x, y, static_cast<uint8_t>(255.999f * r), static_cast<uint8_t>(255.999f * g), static_cast<uint8_t>(255.999f * b));
		}
	}

	writePPM("out.ppm", image);
	std::printf("Escrito out.ppm (%dx%d)\n", width, height);
	return 0;
}
