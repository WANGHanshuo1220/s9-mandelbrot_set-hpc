#include "../fractal.h"

std::string implem_name = "seq";
int vec_size = 1;

void compute_mandelbrot(const int maxiter, const FLOAT centerr, const FLOAT centeri, const FLOAT zoom, uint32_t *pixels)
{
	int x,y,n;
	FLOAT zr,zi,cr,ci;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			FLOAT m=0;
			/* Get the complex poing on gauss space to be calculate */
			zr=cr=centerr + (x - (WIDTH /2))/zoom;
			zi=ci=centeri + (y - (HEIGHT/2))/zoom;
			/* Applies the actual mandelbrot formula on that point */
			for (n = 0; n <= maxiter && m < BAIL_OUT * BAIL_OUT; n++) {
				FLOAT a=zr*zr-zi*zi+cr;
				FLOAT b=zr*(zi+zi)+ci;
				zr=a;
				zi=b;
				m=a*a+b*b;
			}
#ifdef USE_SDL
			/* Paint the pixel calculated depending on the number
			   of iterations found */
			int color;
			if (n<maxiter)
				color=(MAPPING[n%16][0]<<16)+(MAPPING[n%16][1]<<8)+(MAPPING[n%16][2]);
			else
				color=0;
			pixels[y*WIDTH + x] = color;
#else
			pixels[y*WIDTH + x] = (uint32_t)n;
#endif
		}
	}
}
