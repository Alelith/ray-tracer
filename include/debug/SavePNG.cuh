#ifndef DEBUG_SAVE_PNG_HPP
# define DEBUG_SAVE_PNG_HPP

# include <string>
# include <vector>

namespace debug
{
	bool save_png_argb8888(const std::string& filePath, int width, int height, const std::vector<int>& argb8888);
}

#endif /* DEBUG_SAVE_PNG_HPP */
