#ifndef DEBUG_SAVE_PNG_HPP
#define DEBUG_SAVE_PNG_HPP

#include <string>
#include <vector>

namespace debug {

/**
 * @brief Saves an ARGB8888 image buffer as a PNG file.
 *
 * @param filePath Output PNG path.
 * @param width Image width.
 * @param height Image height.
 * @param argb8888 Pixel buffer in ARGB8888 packed format.
 * @return true if the file was written successfully, false otherwise.
 */
bool save_png_argb8888(const std::string& filePath, int width, int height, const std::vector<int>& argb8888);

} // namespace debug

#endif // DEBUG_SAVE_PNG_HPP