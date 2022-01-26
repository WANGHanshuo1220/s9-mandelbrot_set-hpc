# The Mandelbrot set (fractal shapes)

The case study is the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set).

## Install dependencies

On Ubuntu:

    $ sudo apt install g++ cmake libsdl2-dev

## Compilation

Compile the code on Linux/MacOS/MinGW:

    $ mkdir build
    $ cd build
    $ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-Wall -funroll-loops -march=native -fopenmp"
    $ make -j4

## CMake options

The proposed `CMakeLists.txt` file can be adapted.

The `USE_FLOAT` option switches to single floating-point precision (by default this option is `OFF` and the implementations use double precision floating-point):

    $ cmake [...] -DUSE_FLOAT=ON

The `ENABLE_SDL` option enables the SDL output (by default this option is `ON` but if the SDL2 library can't be found on your system then the implementations will be compiled without SDL). To disable the SDL display, do as follow:

    $ cmake [...] -DENABLE_SDL=OFF

Have fun :-)!
