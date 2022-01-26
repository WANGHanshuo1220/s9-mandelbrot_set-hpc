#include "../fractal.h"
#include <iostream>
#include <omp.h>

// add library for vectorilization
#include <immintrin.h>

#ifdef USE_FLOAT 
	// define funcs for float type
	using m  = __m256;
	using mi = __m256i;
	using T  = uint32_t;
	#define inc int(sizeof(__m256)/sizeof(uint32_t))
	#define _mm256_set1 _mm256_set1_ps
	#define _mm256_set1_epi _mm256_set1_epi32
	#define _mm256_set _mm256_set_ps
	#define _mm256_cmpgt_epi _mm256_cmpgt_epi32
	#define _mm256_and _mm256_and_si256
	#define _mm256_cast1 _mm256_castps_si256
	#define _mm256_movemask _mm256_movemask_ps
	#define _mm256_cast2 _mm256_castsi256_ps
	#define _mm256_mul _mm256_mul_ps
	#define _mm256_add _mm256_add_ps
	#define _mm256_sub _mm256_sub_ps
	#define _mm256_add_epi _mm256_add_epi32
	#define _mm256_add _mm256_add_ps
	#define _mm256_cmp _mm256_cmp_ps
#else
	// define funcs for double type
	using m  = __m256d;
	using mi = __m256i;
	using T  = uint64_t;
	#define inc int(sizeof(__m256d)/sizeof(uint64_t))
	#define _mm256_set1 _mm256_set1_pd
	#define _mm256_set1_epi _mm256_set1_epi64x
	#define _mm256_set _mm256_set_pd
	#define _mm256_cmpgt_epi _mm256_cmpgt_epi64
	#define _mm256_and _mm256_and_si256
	#define _mm256_cast1 _mm256_castpd_si256
	#define _mm256_movemask _mm256_movemask_pd
	#define _mm256_cast2 _mm256_castsi256_pd
	#define _mm256_mul _mm256_mul_pd
	#define _mm256_add _mm256_add_pd
	#define _mm256_sub _mm256_sub_pd
	#define _mm256_add_epi _mm256_add_epi64
	#define _mm256_add _mm256_add_pd
	#define _mm256_cmp _mm256_cmp_pd
#endif

std::string implem_name = "simd_mt";
int vec_size = 1;

void compute_mandelbrot(const int maxiter, const FLOAT centerr, const FLOAT centeri, const FLOAT zoom, uint32_t *pixels)
{
	int x,y;
	FLOAT zi, ci;
	FLOAT zr[inc];
	FLOAT cr[inc];
	T buffer[inc]; // used for storeu _n


	// define 256 bits registers for calculation of the mandelbrot point
	m _zr, _zi, _cr, _ci, _a, _b, _a2, _b2, _zr2, _zi2, _m;

	// define 256 bits registers containing constant values
	m _two, _BAIL_OUT; 

	_two = _mm256_set1(2.0);
	_BAIL_OUT = _mm256_set1((BAIL_OUT * BAIL_OUT));

	// define 256 bits registers for "for" loop
	m  _mask1;
	mi _mask2, _mask3, _n, _maxiter, _one;

	_maxiter = _mm256_set1_epi(maxiter);	// initailize _maxiter
	_one 	 = _mm256_set1_epi(1);		// this vector is for incrementing n
	
	#pragma omp parallel private(_n, _zr, _zi, _cr, _ci, _a, _b, _a2, _b2, _zr2, _zi2, _m, _mask1, _mask2, _mask3)
	#pragma omp for schedule(dynamic) nowait
	for (y = 0; y < HEIGHT; y++)
	{
		zi = ci = centeri + (y - (HEIGHT/2)) / zoom;	

		for (x = 0; x < WIDTH; x += inc)
		{
			// FLOAT m=0; 
			// initailize _m and _n
			_m = _mm256_set1(0.0);
			_n = _mm256_set1_epi(0);
			
			/* Get the complex poing on gauss space to be calculate */
			for(int i = 0; i < inc; i++)
			{
				// get inc(4 or 8) values of pixels
				zr[i] = cr[i] = centerr + (x + i - (WIDTH / 2)) / zoom;
			}
			
			// initialize the vectors
#ifdef USE_FLOAT
			_zr = _mm256_set(zr[0], zr[1], zr[2], zr[3], zr[4], zr[5], zr[6], zr[7]);
			_cr = _mm256_set(cr[0], cr[1], cr[2], cr[3], cr[4], cr[5], cr[6], cr[7]);
#else
			_zr = _mm256_set(zr[0], zr[1], zr[2], zr[3]);
			_cr = _mm256_set(cr[0], cr[1], cr[2], cr[3]);
#endif
			_zi = _mm256_set1(zi);
			_ci = _mm256_set1(ci);

			/* Applies the actual mandelbrot formula on that point */

			repeat_for:

			// check loop condition
			_mask1 = _mm256_cmp(_m, _BAIL_OUT, _CMP_LT_OQ);	// compare m with BAIL_OUT * BAIL_OUT
			_mask2 = _mm256_cmpgt_epi(_maxiter, _n);			// compare n with maxiter
			_mask3 = _mm256_and(_mask2, _mm256_cast1(_mask1));	// use "and" to check the loop condition, first cast FLOAT to int

			if (_mm256_movemask(_mm256_cast2(_mask3)) == 0)	// if equal to 0, means neither "n" nor "m" of every element is unsatisfied
				goto out;

			// 	FLOAT a=zr*zr-zi*zi+cr;
			_zr2 = _mm256_mul(_zr, _zr);	// calculate zr * zr ( = zr^2)
			_zi2 = _mm256_mul(_zi, _zi);	// calculate zi * zi ( = zi^2)
			_a   = _mm256_sub(_zr2, _zi2);	// calculate zr * zr - zi * zi ( = a)
			_a   = _mm256_add(_a, _cr);	// calculate zr * zr - zi * zi + cr ( = a)

			// 	FLOAT b=zr*(zi+zi)+ci;
			_b = _mm256_mul(_zr, _zi);	// calculate zr * zi
			_b = _mm256_mul(_b, _two);	// calculate zr * zi * 2.0
			_b = _mm256_add(_b, _ci);	// calculate zr * zi * 2.0 + ci

			// 	zr=a;
			_zr = _a;

			// 	zi=b;
			_zi = _b;

			// 	m=a*a+b*b;
			_a2 = _mm256_mul(_a, _a);	// calculate a * a ( = a^2)
			_b2 = _mm256_mul(_b, _b);	// calculate b * b ( = b^2)
			_m  = _mm256_add(_a2, _b2);	// calculate a^2 + b^2 ( = m)

			_n	   = _mm256_add_epi(_n, _mm256_and(_mask3, _one));		// incrementing n

			goto repeat_for;

			out:

			// get the values stored in _n
			_mm256_storeu_si256((__m256i*)buffer, _n);

#ifdef USE_SDL
			/* Paint the pixel calculated depending on the number
			of iterations found */
			int color;

			// add a for loop to set color for each of these pixels
			for(int i = 0; i < inc; i++)
			{
				if (buffer[i] < T(maxiter))
					color=(MAPPING[T(buffer[inc-1-i])%16][0]<<16)+(MAPPING[T(buffer[inc-1-i])%16][1]<<8)+(MAPPING[T(buffer[inc-1-i])%16][2]);
				else
					color=0;
				
				pixels[y*WIDTH + x + i] = color;
			}
#else
			for(int i = 0; i < inc; i++)
			{
				pixels[y*WIDTH + x + i] = T(buffer[inc-1-i]);
			}
#endif
		}
	}
}
