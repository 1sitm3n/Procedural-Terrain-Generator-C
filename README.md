# Procedural Terrain Generator (C, Perlin Noise, OpenMP)

This project generates large terrain heightmaps using 2D Perlin noise and parallel processing with OpenMP. It showcases:

- Parallelising mathematical operations on large arrays
- Cache-friendly memory access (row-major traversal)
- File I/O optimisation via buffered binary writes
- Parallel efficiency comparison between **static** and **dynamic** OpenMP scheduling

## Build

Requires a C compiler with OpenMP support (e.g. `gcc`):

    make

This produces:

    ./terrain_generator

Usage:

    ./terrain_generator [width] [height] [threads] [scale] [octaves] [chunk_size]

Example:

    ./terrain_generator 4096 4096 8 8.0 6 64


Outputs:

terrain_static.pgm – generated with schedule(static, chunk_size)
terrain_dynamic.pgm – generated with schedule(dynamic, chunk_size)

The program prints timing information for both runs and reports the relative speedup.
You can open the .pgm files in most image viewers or import them into tools such as GIMP or MATLAB.

**Parallelisation details**

Heightmap stored as a 1D float array in row-major order (heightmap[y * width + x]).
Outer loop parallelises over rows y; inner loop iterates over x for good cache locality.
OpenMP uses schedule (runtime) in the loop; omp_set_schedule switches between static and dynamic scheduling at runtime.
Noise evaluation uses multi-octave fractional Brownian motion built on top of 2D Perlin noise.

**Build and test locally**

In the repo:
make
./terrain_generator 1024 1024 8 8.0 6 64 (system friendly example)
