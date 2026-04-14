#define STB_IMAGE_IMPLEMENTATION
#include "Texture.cuh"

namespace render
{

	Texture::Texture() : width(0), height(0), image(0) {}

	Texture::Texture(const char *path)
	{
		int	channels;
		unsigned char	*data = stbi_load(path, &width, &height, &channels, 4);

		if (!data)
		{
			printf("Failed to load texture: %s\n", path);
			width = height = 0;
			image = 0;
			return;
		}

		cudaChannelFormatDesc	channelDesc = cudaCreateChannelDesc<uchar4>();
		cudaArray	*cuArray;

		cudaMallocArray(&cuArray, &channelDesc, width, height);
		cudaMemcpyToArray(cuArray, 0, 0, data, width * height * 4, cudaMemcpyHostToDevice);

		cudaResourceDesc	resDesc = {};
		resDesc.resType = cudaResourceTypeArray;
		resDesc.res.array.array = cuArray;

		cudaTextureDesc	texDesc = {};
		texDesc.addressMode[0] = cudaAddressModeWrap;
		texDesc.addressMode[1] = cudaAddressModeWrap;
		texDesc.filterMode = cudaFilterModeLinear;
		texDesc.readMode = cudaReadModeNormalizedFloat;
		texDesc.normalizedCoords = 1;

		cudaCreateTextureObject(&image, &resDesc, &texDesc, nullptr);
		stbi_image_free(data);
	}

} // namespace render
