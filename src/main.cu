#include "Camera.cuh"
#include "Timer.cuh"
#include "LinearBVH.cuh"
#include "debug/PrintPPM.cuh"
#include "debug/SavePNG.cuh"
#include "parsing/Parser.cuh"

#include <fstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>

#include <SDL2/SDL.h>

using std::cout;
using std::endl;

static bool ensureRendersDirectory()
{
	struct stat st;
	if (stat("renders", &st) == 0)
		return S_ISDIR(st.st_mode);

	if (mkdir("renders", 0755) == 0)
		return true;

	return errno == EEXIST;
}

static std::string makeRenderOutputPath()
{
	std::time_t now = std::time(nullptr);
	std::tm localTime;
	localtime_r(&now, &localTime);

	std::ostringstream oss;
	oss << "renders/" << std::put_time(&localTime, "%d%m%y_%H%M%S") << ".png";
	return oss.str();
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "Uso: " << argv[0] << " <archivo.lrtc>" << std::endl;
		return 1;
	}

	Parser parser(argv[1]);
	if (!parser.parse())
	{
		std::cerr << "Error al parsear el archivo: " << argv[1] << std::endl;
		return 1;
	}

	ParsedCameraData camData = parser.getCameraData();
	if (!camData.isParsed)
	{
		std::cerr << "Error: No se encontró la configuración de la cámara en el archivo .lrtc" << std::endl;
		return 1;
	}

	WorldGeometries worldGeoms = parser.getWorldGeometries();
	Materials materials = parser.getMaterials();

	std::cout << "Creating BVH..." << std::endl;
LinearBVH bvh(worldGeoms);
std::cout << "BVH Created!" << std::endl;
	Camera camera(camData.aspectRatio, camData.imageWidth, camData.samplesPerPixel, camData.maxDepth, camData.fov, 
				  camData.position, camData.normal, bvh, materials, 
				  camData.normalWeight, camData.positionWeight, camData.albedoWeight, camData.colorWeight);

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	// Utilizar las dimensiones reales resultantes de la cámara (por un lado window display vs resolución actual)
	const int imageWidth = camData.imageWidth;
	const int imageHeight = static_cast<int>(imageWidth / camData.aspectRatio);

	SDL_Window* win = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
	if (win == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
	{
		SDL_DestroyWindow(win);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, imageWidth, imageHeight);
	if (texture == nullptr)
	{
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	Timer timer;
	int spp = camData.samplesPerPixel;
	
	if (!ensureRendersDirectory())
	{
		std::cerr << "No se pudo crear/acceder al directorio renders/." << std::endl;
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}
	int i = 0;
	while (true)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				SDL_DestroyTexture(texture);
				SDL_DestroyRenderer(ren);
				SDL_DestroyWindow(win);
				std::cerr << "\nRendering stopped by user." << std::endl;
				SDL_Quit();
				return 0;
			}
		}
		if (i < spp)
		{
			std::cerr << "\rSample " << (i + 1) << "/" << spp << " rendering." << std::flush;
			timer.start();
			camera.renderSample();
			timer.stop();
			
			std::cerr << " [Render time: " << timer.elapsedMilliseconds() << " ms]" << std::flush;

			vector<int> imageData = camera.getImage();
			SDL_UpdateTexture(texture, nullptr, imageData.data(), imageWidth * sizeof(int));
			i++;

			if (i == spp)
			{
				const std::string outputPath = makeRenderOutputPath();
				if (debug::save_png_argb8888(outputPath, imageWidth, imageHeight, imageData))
					std::cerr << "\nPNG guardado en: " << outputPath << std::endl;
				else
					std::cerr << "\nError al guardar PNG en: " << outputPath << std::endl;
			}
		}
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, texture, nullptr, nullptr);
		SDL_RenderPresent(ren);
	}

	std::cerr << endl;

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

	SDL_Quit();
	return 0;
}
