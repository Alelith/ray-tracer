#include "PrintPPM.cuh"
#include <iostream>

namespace debug
{
	void print_ppm(int width, int height, const std::vector<int>& trgb_colors)
	{
		std::cout << "P3\n" << width << " " << height << "\n255\n";
		for (int color : trgb_colors)
		{
			int r = (color >> 16) & 0xFF;
			int g = (color >> 8) & 0xFF;
			int b = color & 0xFF;
			std::cout << r << " " << g << " " << b << "\n";
		}
	}
}
