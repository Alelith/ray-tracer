#ifndef IMAGE_HPP
# define IMAGE_HPP

# include <vector>
# include <cstdint>
# include <string>

struct Image {
	int		width = 0;
	int		height = 0;
	std::vector<uint8_t>	rgb;

	inline Image(int width_, int height_);

	inline void		set(int x, int y, uint8_t r, uint8_t g, uint8_t b);
};

inline bool		writePPM(const std::string& path, const Image& img);

namespace png_detail {

	inline uint32_t crc32(const uint8_t* data, size_t n, uint32_t crc = 0);
	inline void putU32BE(std::vector<uint8_t>& v, uint32_t x);
	inline void writeChunk(FILE* f, const char type[4], const std::vector<uint8_t>& data);

}  // namespace png_detail

inline bool writePNG(const std::string& path, const Image& img);

# include "detail/Image.ipp"

#endif // IMAGE_HPP
