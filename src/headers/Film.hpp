#ifndef FILM_HPP
# define FILM_HPP

# include <vector>
# include "Image.hpp"
# include "Vec3.hpp"

struct Film {
	public:
		int		width = 0;
		int		height = 0;
		std::vector<Color> accum;

		inline Film(int width_, int height_);

		inline int		pixelCount() const;
		inline void		add(uint32_t pixel, const Color& color);

		inline Image	resolve(int samplesPerPixel) const;

	private:
		static inline float		linearToSRGB(float v);
		static inline uint8_t	toByte(float linear);
};

# include "detail/Film.ipp"

#endif // FILM_HPP
