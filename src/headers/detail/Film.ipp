#include <algorithm>

inline Film::Film(int width_, int height_) : width(width_), height(height_), accum(size_t(width_) * height_, Color()) {}

inline int Film::pixelCount() const { return width * height; }
inline void Film::add(uint32_t pixel, const Color& color) { accum[pixel] += color; }

inline Image Film::resolve(int samplesPerPixel) const {
	Image img(width, height);
	float scale = 1.0f / static_cast<float>(samplesPerPixel);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Color c = accum[static_cast<size_t>(y) * width + x] * scale;
			img.set(x, y, toByte(c.x), toByte(c.y), toByte(c.z));
		}
	}
}

inline float Film::linearToSRGB(float v) {
	v = std::clamp(v, 0.0f, 1.0f);
	return v <= 0.0031308f ? 12.92f * v : 1.055f * std::powf(v, 1.0f / 2.4f) - 0.055f;
}
inline uint8_t Film::toByte(float linear) {
	return static_cast<uint8_t>(255.999f * linearToSRGB(linear));
}