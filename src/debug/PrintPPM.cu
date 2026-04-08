#include "debug/PrintPPM.cuh"
#include <iostream>

namespace debug {

void print_ppm(int width, int height, const std::vector<int>& trgb_colors) {
    // PPM header: P3 (ASCII RGB), width, height, max color value (255)
    std::cout << "P3\n" << width << " " << height << "\n255\n";

    // Print each color
    for (int color : trgb_colors) {
        // Assuming TRGB format (Transparency, Red, Green, Blue) in 32 bits:
        // T: bits 24-31, R: bits 16-23, G: bits 8-15, B: bits 0-7
        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = color & 0xFF;

        std::cout << r << " " << g << " " << b << "\n";
    }
}

} // namespace debug
