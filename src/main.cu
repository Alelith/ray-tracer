#include "Camera.cuh"
#include "Timer.cuh"
#include "LinearBVH.cuh"
#include "debug/PrintPPM.cuh"
#include "debug/SavePNG.cuh"

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

int main()
{
	WorldGeometries worldGeoms;
	Materials materials;

	makeSphere(Point3( 0.0,   0.5,  -2), 0.125, worldGeoms);   // centre
	makeTorus(1.25, 0.5, Point3(0.0, 0.0, -2), Vector3(0, 1, 0), worldGeoms); // centre
	makeCylinder(0.5, 0.75, Point3(3.0, 0.0, -1), Vector3(0.25, 1, -0.25), worldGeoms);   // right
	makeSphere(Point3( 3.7,   1.0,  -1),  0.5, worldGeoms);   // right
	makeSphere(Point3( -3.0,   0.0,  -1),  0.5, worldGeoms);   // left
	makeCone(0.5, 1.0, Point3(-4, 2, -1), Vector3(-0.25, -1, 0.25), worldGeoms);   // left
	makeSphere(Point3( 0.0, -100.5, -1), 100.0, worldGeoms);  // ground
	makeSphere(Point3( 0.0,   2.0,  -2), 0.25, worldGeoms);   // above
	makeSphere(Point3( -1.5,   2.0, -2), 0.25, worldGeoms);   // above
	makeSphere(Point3( 1.5,   2.0,  -2), 0.25, worldGeoms);   // above
	makeTorus(0.75, 0.125, Point3( 0.0,   1.5,  -2), Vector3(0, 1, 0), worldGeoms);   // above

	addMaterial(Color(0.25, 0.75, 1), 0.125, 1.0, 0.0, 0.0, Color(0.0, 0.0, 0.0), 1.0, materials); // centre
	addMaterial(Color(0, 0.5, 1), 0.0625, 0.0, 1.0, 4.5, Color(0.0, 0.0, 0.0), 1.0, materials); // centre
	addMaterial(Color(1, 0, 1), 0.125, 1.0, 0.0, 0.0, Color(0.0, 0.0, 0.0), 1.0, materials); // right
	addMaterial(Color(1, 0.75, 0.5), 1, 1.0, 0.0, 0.0, Color(0.0, 0.0, 0.0), 1.0, materials); // right
	addMaterial(Color(1, 1, 1), 1.0, 0.0, 0.0, 1.0, Color(0.0, 0.0, 0.0), 1.0, materials); // left
	addMaterial(Color(0.6, 0.75, 0.3), 1, 1.0, 0.0, 0.0, Color(0.0, 0.0, 0.0), 1.0, materials); // left
	addMaterial(Color(1.0, 1, 0.5), 1.0, 0.0, 0.0, 1.0, Color(0.0, 0.0, 0.0), 1.0, materials); // ground
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color(1.0, 1.0, 1.0), 70.0, materials); // above
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color(0.0, 1.0, 1.0), 25.0, materials); // above
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color(1.0, 1.0, 0.0), 25.0, materials); // above
	addMaterial(Color(0, 0, 0), 0.0, 0.0, 1.0, 1.5, Color(1.0, 0.01, 1.0), 60.0, materials); // above

	LinearBVH bvh(worldGeoms);
	Camera camera(16.0 / 9.0, 3840, 25, 25, 90, Point3(0, 2.75, -8), Vector3(0.0, -0.75, 1.0), bvh, materials, 128.0, 0.05, 0.005, 0.25);

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

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

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 3840, 2160);
	if (texture == nullptr)
	{
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	Timer timer;
	int spp = 2500;
	const int imageWidth = 3840;
	const int imageHeight = 2160;
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
