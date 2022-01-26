#ifndef FRACTAL_H_
#define FRACTAL_H_

#include <cstdint>
#include <string>

#include "constants.h"

extern std::string implem_name;
extern int         vec_size;

void compute_mandelbrot(const int maxiter, const FLOAT centerr, const FLOAT centeri, const FLOAT zoom, uint32_t *pixels);

#endif
