#ifndef DEBUG_PRINT_PPM_HPP
# define DEBUG_PRINT_PPM_HPP

# include <vector>

namespace debug
{
	void print_ppm(int width, int height, const std::vector<int>& trgb_colors);
}

#endif /* DEBUG_PRINT_PPM_HPP */
