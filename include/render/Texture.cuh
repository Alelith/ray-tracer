#ifndef TEXTURE_CUH
# define TEXTURE_CUH

# include "Vector3.cuh"
# include "stb_image.h"

struct Texture
{
	int		width;
	int		height;

	cudaTextureObject_t image;

	Texture();
	Texture(const char *path);
};

#endif /* TEXTURE_CUH */
