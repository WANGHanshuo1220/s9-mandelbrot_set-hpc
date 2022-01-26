#ifdef USE_SDL
#include <SDL.h>
#endif

#include <cstdint>
#include <math.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <unistd.h>
#include "constants.h"
#include "fractal.h"

unsigned char MAPPING[16][3]= {{66,  30, 15},
                               {25,   7, 26},
                               {9,    1, 47},
                               {4,    4, 73},
                               {0,    7,100},
                               {12,  44,138},
                               {24,  82,177},
                               {57, 125,209},
                               {134,181,229},
                               {211,236,248},
                               {241,233,191},
                               {248,201, 95},
                               {255,170,  0},
                               {204,128,  0},
                               {153, 87,  0},
                               {106, 52,  3}};

int main(int argc, char **argv)
{
	/* Initialize variables */
	int nIterations = 150;
	if (argc >= 2)
		nIterations = std::atoi(argv[1]);
	FLOAT zoom = START_ZOOM;
//	FLOAT centerr = -0.800671;
//	FLOAT centeri = -0.158392;
	FLOAT centerr = 0.001643721971153;
	FLOAT centeri = 0.822467633298876;

	std::clog << "--------------------- "                                     << std::endl;
	std::clog << "Mandelbrot parameters "                                     << std::endl;
	std::clog << "--------------------- "                                     << std::endl;
	std::clog << "global iterations:    " << nIterations                      << std::endl;
	std::clog << "start zoom:           " << zoom                             << std::endl;
	std::clog << "zoom factor:          " << ZOOM_FACTOR                      << std::endl;
	std::clog << "bail out:             " << BAIL_OUT                         << std::endl;
	std::clog << "center point:         " << centerr << " " << centeri << "i" << std::endl;
	std::clog << "dimensions (pixels):  " << WIDTH << "x" << HEIGHT           << std::endl;
#ifdef USE_FLOAT
	std::clog << "floating-point prec.: " << "simple (32-bit)"                << std::endl;
#else
	std::clog << "floating-point prec.: " << "double (64-bit)"                << std::endl;
#endif
	std::clog                                                                 << std::endl;

#ifdef USE_SDL
	/* SDL SEtup */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
		std::exit(1);
	}

	atexit(SDL_Quit);
	SDL_Window *sdlWindow;
	SDL_Renderer *renderer;

	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &sdlWindow, &renderer))
	{
		std::cerr << "Couldn't create window and renderer: " << SDL_GetError() << std::endl;
		std::exit(1);
	}
	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH,HEIGHT);
#endif

	// std::this_thread::sleep_for(std::chrono::seconds(1));
	std::vector<uint32_t> pixels(WIDTH*HEIGHT*2);

	std::clog << "---------------" << std::endl;
	std::clog << "Simulation info" << std::endl;
	std::clog << "---------------" << std::endl;
	auto t_start = std::chrono::steady_clock::now();
	for (int i = 0; i < nIterations; i++)
	{
		int maxiter = (WIDTH/2) * 0.049715909 * log10(zoom);

		std::clog << "global iteration: "    << i       << std::endl;
		std::clog << "zoom: "                << zoom    << std::endl;
		std::clog << "internal iterations: " << maxiter << std::endl;
		std::clog << "----"                             << std::endl;

		compute_mandelbrot(maxiter, centerr, centeri, zoom, pixels.data());

		zoom *= ZOOM_FACTOR;

#ifdef USE_SDL
	SDL_UpdateTexture(texture, NULL, pixels.data(), WIDTH*sizeof(uint32_t));
	SDL_RenderClear  (renderer                                            );
	SDL_RenderCopy   (renderer, texture, NULL, NULL                       );
	SDL_RenderPresent(renderer                                            );
#endif
	}
	auto duration = std::chrono::steady_clock::now() - t_start;

	auto time_sec = duration.count() * 0.000000001f;
	std::clog << "Total time = " << duration.count() * 0.000000001f << " sec" << std::endl;

#ifdef USE_SDL
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
#endif

	std::clog                 << std::endl;
	std::clog << "----------" << std::endl;
	std::clog << "CSV output" << std::endl;
	std::clog << "----------" << std::endl;
	std::clog << "\"implementation name\",\"number of iterations\",\"vector size\",\"floating-point precision\",\"execution time (sec)\"" << std::endl;
	int prec = 0;
#ifdef USE_FLOAT
	prec = 32;
#else
	prec = 64;
#endif
	std::clog << implem_name << ", " << nIterations << ", " << vec_size << ", " << prec << ", " << time_sec << std::endl;

	return 0;
}
