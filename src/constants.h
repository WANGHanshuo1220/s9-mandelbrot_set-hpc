#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#ifdef USE_FLOAT
using FLOAT = float;
#else
using FLOAT = double;
#endif

#define WIDTH 1200
#define HEIGHT 800

#define START_ZOOM  (WIDTH * 0.25296875f)
#define BAIL_OUT    2.0
//#define ZOOM_FACTOR 1.4
#define ZOOM_FACTOR 1.2

extern unsigned char MAPPING[16][3];

#endif
