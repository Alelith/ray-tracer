#include "SavePNG.cuh"
#include <cstdint>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace debug
{
	bool save_png_argb8888(const std::string& filePath, int width, int height, const std::vector<int>& argb8888)
	{
		if (width <= 0 || height <= 0)
			return false;
		const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
		if (argb8888.size() < pixelCount)
			return false;
		std::vector<unsigned char> rgba(pixelCount * 4);
		for (size_t i = 0; i < pixelCount; ++i)
		{
			const uint32_t px = static_cast<uint32_t>(argb8888[i]);
			rgba[i * 4 + 0] = static_cast<unsigned char>((px >> 16) & 0xFF);
			rgba[i * 4 + 1] = static_cast<unsigned char>((px >> 8) & 0xFF);
			rgba[i * 4 + 2] = static_cast<unsigned char>(px & 0xFF);
			rgba[i * 4 + 3] = static_cast<unsigned char>((px >> 24) & 0xFF);
		}
		return stbi_write_png(filePath.c_str(), width, height, 4, rgba.data(), width * 4) != 0;
	}
}
