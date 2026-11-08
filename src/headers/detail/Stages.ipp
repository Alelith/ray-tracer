# include "Parallel.hpp"

inline void stageGenerateCamera(const Camera& cam, int width, int height, PathQueue& out) {
	out.clear();
	parallelFor(static_cast<uint32_t>(width * height), [&](uint32_t pixelIndex) {
		int x = static_cast<int>(pixelIndex) % width;
		int y = static_cast<int>(pixelIndex) / width;

		float s = (static_cast<float>(x) + 0.5f) / static_cast<float>(width);
		float t = 1.0f - (static_cast<float>(y) + 0.5) / static_cast<float>(height);

		uint32_t slot = out.append();
		out.setRay(slot, cam.getRay(s, t));
		out.pixel[slot] = pixelIndex;
	});
}
inline Color	skyColor(const Vec3& dir) {
	float t = 0.5f * (dir.y + 1.0f);
	return lerp(Color(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f), t);
}
inline void		stageMissSky(const PathQueue& in, Film& film) {
	parallelFor(in.size(), [&](uint32_t i) {
		Ray r = in.ray(i);
		film.add(in.pixel[i], skyColor(r.dir));
	});
}
