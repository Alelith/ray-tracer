#include "Camera.cuh"
#include "Timer.cuh"
#include "LinearBVH.cuh"
#include "debug/PrintPPM.cuh"

#include <fstream>
#include <cmath>
#include <algorithm>
#include <iostream>

#include <SDL2/SDL.h>

using std::cout;
using std::endl;

int main()
{
	WorldGeometries worldGeoms;
	Materials materials;

	makeSphere(Point3( 0.0,   0.0,  -2), 0.5, worldGeoms);   // centre
	makeSphere(Point3( 3.0,   0.0,  -1),  0.5, worldGeoms);   // right
	makeSphere(Point3( -3.0,   0.0,  -1),  0.5, worldGeoms);   // left
	makeSphere(Point3( 0.0, -100.5, -1), 100.0, worldGeoms);  // ground
	makeSphere(Point3( 0.0,   1.0,  -0.5), 0.5, worldGeoms);   // above
	makeSphere(Point3( -1.5,   1.0, 0), 0.25, worldGeoms);   // above
	makeSphere(Point3( 1.5,   1.0,  0), 0.25, worldGeoms);   // above

	addMaterial(Color(0, 1, 0), 0.0, 0.0, 1.0, 4.5, Color(0.0, 0.0, 0.0), 1.0, materials); // centre
	addMaterial(Color(1, 0, 1), 0.5, 1.0, 0.0, 0.0, Color(0.0, 0.0, 0.0), 1.0, materials); // right
	addMaterial(Color(1, 1, 1), 1.0, 0.0, 0.0, 1.0, Color(0.0, 0.0, 0.0), 1.0, materials); // left
	addMaterial(Color(0.0, 0.8, 0.8), 1.0, 0.0, 0.0, 0.0, Color(0.0, 0.0, 0.0), 1.0, materials); // ground
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color(1.0, 0.0, 1.0), 50.0, materials); // above
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color(0.0, 1.0, 1.0), 25.0, materials); // above
	addMaterial(Color(1, 1, 1), 0.0, 0.0, 1.0, 1.5, Color(0.0, 1.0, 1.0), 25.0, materials); // above

	LinearBVH bvh(worldGeoms);
	Camera camera(16.0 / 9.0, 1920, 25, 25, 90, Point3(0, 2, -7), Vector3(0.0, -0.5, 1.0), bvh, materials);

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

	SDL_Texture* texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 1920, 1080);
	if (texture == nullptr)
	{
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	Timer timer;
	int spp = 10000;
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
			int width = 1920;
			SDL_UpdateTexture(texture, nullptr, imageData.data(), width * sizeof(int));
			i++;
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
