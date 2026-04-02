#ifndef DEBUG_PRINT_PPM_HPP
#define DEBUG_PRINT_PPM_HPP

#include <vector>

namespace debug {

/**
 * @brief Prints an image in PPM (P3) format to standard output.
 * 
 * @param width The width of the image.
 * @param height The height of the image.
 * @param trgb_colors A vector containing the image colors compressed in an int (TRGB format).
 */
void print_ppm(int width, int height, const std::vector<int>& trgb_colors);

} // namespace debug

#endif // DEBUG_PRINT_PPM_HPP
