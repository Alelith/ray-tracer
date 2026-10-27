Image::Image(int width_, int height_) : width(width_), height(height_), rgb(size_t(width_) * height_ * 3, 0) {}

void Image::set(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	size_t i = (size_t(y) * width + x) * 3;
	rgb[i] = r;
	rgb[i + 1] = g;
	rgb[i + 2] = b;
}

bool writePPM(const std::string& path, const Image& img) {
	FILE* f = std::fopen(path.c_str(), "wb");
	if (!f) return false;

	std::fprintf(f, "P6\n%d %d\n255\n", img.width, img.height);
	std::fwrite(img.rgb.data(), 1, img.rgb.size(), f);
	std::fclose(f);
	return true;
}
