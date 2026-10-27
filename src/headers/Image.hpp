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

# include "detail/Image.ipp"

#endif // IMAGE_HPP
