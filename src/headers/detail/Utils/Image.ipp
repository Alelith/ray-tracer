inline Image::Image(int width_, int height_) : width(width_), height(height_), rgb(size_t(width_) * height_ * 3, 0) {}

inline void Image::set(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	size_t i = (size_t(y) * width + x) * 3;
	rgb[i] = r;
	rgb[i + 1] = g;
	rgb[i + 2] = b;
}

inline bool writePPM(const std::string& path, const Image& img) {
	FILE* f = std::fopen(path.c_str(), "wb");
	if (!f) return false;

	std::fprintf(f, "P6\n%d %d\n255\n", img.width, img.height);
	std::fwrite(img.rgb.data(), 1, img.rgb.size(), f);
	std::fclose(f);
	return true;
}

namespace png_detail {

	inline uint32_t crc32(const uint8_t* data, size_t n, uint32_t crc) {
		static uint32_t table[256];
		static bool ready = false;
		if (!ready) {
			for (uint32_t i = 0; i < 256; ++i) {
				uint32_t c = i;
				for (int k = 0; k < 8; ++k) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
				table[i] = c;
			}
			ready = true;
		}
		crc = ~crc;
		for (size_t i = 0; i < n; ++i) crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
		return ~crc;
	}

	inline void putU32BE(std::vector<uint8_t>& v, uint32_t x) {
		v.push_back(uint8_t(x >> 24));
		v.push_back(uint8_t(x >> 16));
		v.push_back(uint8_t(x >> 8));
		v.push_back(uint8_t(x));
	}

	inline void writeChunk(FILE* f, const char type[4], const std::vector<uint8_t>& data) {
		std::vector<uint8_t> buf;
		putU32BE(buf, uint32_t(data.size()));
		buf.insert(buf.end(), type, type + 4);
		buf.insert(buf.end(), data.begin(), data.end());
		// El CRC cubre el tipo y los datos, pero no la longitud.
		uint32_t crc = crc32(buf.data() + 4, buf.size() - 4);
		putU32BE(buf, crc);
		std::fwrite(buf.data(), 1, buf.size(), f);
	}

}  // namespace png_detail

inline bool writePNG(const std::string& path, const Image& img) {
	using namespace png_detail;
	FILE* f = std::fopen(path.c_str(), "wb");
	if (!f) return false;

	const uint8_t signature[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
	std::fwrite(signature, 1, 8, f);

	std::vector<uint8_t> ihdr;
	putU32BE(ihdr, uint32_t(img.width));
	putU32BE(ihdr, uint32_t(img.height));
	ihdr.push_back(8);  // bits por canal
	ihdr.push_back(2);  // tipo de color: RGB
	ihdr.push_back(0);  // compresión
	ihdr.push_back(0);  // filtro
	ihdr.push_back(0);  // sin entrelazado
	writeChunk(f, "IHDR", ihdr);

	// Cada fila va precedida por un byte de filtro (0 = sin filtro).
	std::vector<uint8_t> raw;
	raw.reserve(size_t(img.height) * (size_t(img.width) * 3 + 1));
	for (int y = 0; y < img.height; ++y) {
		raw.push_back(0);
		const uint8_t* row = &img.rgb[size_t(y) * img.width * 3];
		raw.insert(raw.end(), row, row + size_t(img.width) * 3);
	}

	// Contenedor zlib con bloques sin comprimir.
	std::vector<uint8_t> z = {0x78, 0x01};
	uint32_t a = 1, b = 0;  // Adler-32
	for (uint8_t byte : raw) {
		a = (a + byte) % 65521;
		b = (b + a) % 65521;
	}
	size_t pos = 0;
	do {
		size_t n = std::min<size_t>(65535, raw.size() - pos);
		bool last = (pos + n == raw.size());
		z.push_back(last ? 1 : 0);
		z.push_back(uint8_t(n & 0xFF));
		z.push_back(uint8_t(n >> 8));
		z.push_back(uint8_t(~n & 0xFF));
		z.push_back(uint8_t((~n >> 8) & 0xFF));
		z.insert(z.end(), raw.begin() + pos, raw.begin() + pos + n);
		pos += n;
	} while (pos < raw.size());
	putU32BE(z, (b << 16) | a);
	writeChunk(f, "IDAT", z);

	writeChunk(f, "IEND", {});
	std::fclose(f);
	return true;
}
