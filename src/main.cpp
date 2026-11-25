#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Camera.hpp"
#include "Film.hpp"
#include "Megakernel.hpp"
#include "Parallel.hpp"
#include "Profiler.hpp"
#include "Scenes.hpp"
#include "WavefrontRenderer.hpp"

static void printUsage() {
	std::printf(
		"Uso: rt [opciones]\n"
		"  --width N --height N   resolución (por defecto 800x450)\n"
		"  --spp N				muestras por píxel (por defecto 64)\n"
		"  --depth N			  rebotes máximos (por defecto 24)\n"
		"  --threads N			hilos (por defecto: todos los núcleos)\n"
		"  --batch N			  máximo de caminos en vuelo (limita la memoria)\n"
		"  --out FICHERO		  imagen de salida (por defecto out.png)\n"
		"  --mesh FICHERO		 malla OBJ (por defecto assets/torusknot.obj)\n"
		"  --megakernel		   usa el renderer megakernel de referencia\n"
		"  --no-nee			   desactiva el muestreo directo de luces\n"
		"  --no-rr				desactiva la ruleta rusa\n"
		"  --no-bvh			   intersección por fuerza bruta\n"
		"  --profile			  muestra el tiempo por etapa\n");
}

int main(int argc, char** argv) {
	RenderSettings cfg;
	int spp = 64;
	bool megakernel = false, useBVH = true, profile = false;
	std::string out = "out.png";
	std::string meshPath = "assets/dragon.obj";

	for (int i = 1; i < argc; ++i) {
		auto is = [&](const char* name) { return !std::strcmp(argv[i], name); };
		auto next = [&]() { return i + 1 < argc ? argv[++i] : (std::fprintf(stderr, "Falta un valor\n"), std::exit(1), nullptr); };
		if (is("--width")) cfg.width = std::atoi(next());
		else if (is("--height")) cfg.height = std::atoi(next());
		else if (is("--spp")) spp = std::atoi(next());
		else if (is("--depth")) cfg.maxDepth = std::atoi(next());
		else if (is("--threads")) ThreadPool::instance().setThreadCount(unsigned(std::atoi(next())));
		else if (is("--batch")) cfg.maxPathsInFlight = uint32_t(std::atoi(next()));
		else if (is("--out")) out = next();
		else if (is("--mesh")) meshPath = next();
		else if (is("--megakernel")) megakernel = true;
		else if (is("--no-nee")) cfg.useNEE = false;
		else if (is("--no-rr")) cfg.useRussianRoulette = false;
		else if (is("--no-bvh")) useBVH = false;
		else if (is("--profile")) profile = true;
		else { printUsage(); return is("--help") ? 0 : 1; }
	}
	Profiler::instance().enabled = profile;

	Scene scene;
	Camera camera(Point3(0, 0, 1), Point3(0, 0, 0), Vec3(0, 1, 0), 40.0f, 1.0f);
	if (!buildCoverScene(scene, meshPath, float(cfg.width) / float(cfg.height), camera)) {
		std::fprintf(stderr, "No puedo cargar la malla %s\n", meshPath.c_str());
		return 1;
	}
	scene.finalize(useBVH);
	std::printf("%zu primitivas, %zu nodos BVH, %u hilos\n", scene.primitiveCount(), scene.bvhNodeCount(),
				ThreadPool::instance().threadCount());

	Film film(cfg.width, cfg.height);
	WavefrontRenderer renderer(scene, camera, cfg);

	auto t0 = std::chrono::steady_clock::now();
	for (int s = 0; s < spp; ++s) {
		if (megakernel)
			renderMegakernelPass(scene, camera, cfg, uint32_t(s), film);
		else
			renderer.renderPass(uint32_t(s), film);
		if ((s + 1) % std::max(1, spp / 10) == 0) {  // como mucho ~10 mensajes de progreso
			std::printf("muestra %d/%d\n", s + 1, spp);
			std::fflush(stdout);
		}
	}
	double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

	writePNG(out, film.resolve(spp));
	std::printf("%s: %dx%d, %d spp, %s, %.2f s\n", out.c_str(), cfg.width, cfg.height, spp,
				megakernel ? "megakernel" : "wavefront", secs);
	if (profile && !megakernel) Profiler::instance().print();
	return 0;
}
