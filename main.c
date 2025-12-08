/* main.c
 *
 * Procedural Terrain Generator (Noise + Parallelisation)
 *
 * Usage:
 *   ./terrain_generator [width] [height] [threads] [scale] [octaves] [chunk_size]
 *
 * Example:
 *   ./terrain_generator 4096 4096 8 8.0 6 64
 *
 * Generates two PGM files:
 *   terrain_static.pgm
 *   terrain_dynamic.pgm
 *
 * and prints timing information for static vs dynamic OpenMP scheduling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "terrain.h"
#include "noise.h"
#include "io.h"

int main(int argc, char **argv) {
    int width      = 2048;
    int height     = 2048;
    int threads    = 8;
    float scale    = 8.0f;
    int octaves    = 6;
    int chunk_size = 64;

    if (argc >= 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }
    if (argc >= 4) {
        threads = atoi(argv[3]);
    }
    if (argc >= 5) {
        scale = (float)atof(argv[4]);
    }
    if (argc >= 6) {
        octaves = atoi(argv[5]);
    }
    if (argc >= 7) {
        chunk_size = atoi(argv[6]);
    }

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Width and height must be positive integers.\n");
        return EXIT_FAILURE;
    }

    printf("Procedural Terrain Generator\n");
    printf("Resolution : %d x %d\n", width, height);
    printf("Threads    : %d\n", threads);
    printf("Scale      : %.2f\n", scale);
    printf("Octaves    : %d\n", octaves);
    printf("Chunk size : %d\n\n", chunk_size);

    float *heightmap_static = (float *)malloc((size_t)width * (size_t)height * sizeof(float));
    float *heightmap_dynamic = (float *)malloc((size_t)width * (size_t)height * sizeof(float));

    if (!heightmap_static || !heightmap_dynamic) {
        fprintf(stderr, "Failed to allocate memory for heightmaps.\n");
        free(heightmap_static);
        free(heightmap_dynamic);
        return EXIT_FAILURE;
    }

    unsigned int seed = (unsigned int)time(NULL);
    noise_init(seed);

    double t_static = 0.0;
    double t_dynamic = 0.0;

    printf("Generating terrain with STATIC schedule...\n");
    generate_heightmap(heightmap_static,
                       width,
                       height,
                       scale,
                       octaves,
                       2.0f,   /* lacunarity */
                       0.5f,   /* gain */
                       threads,
                       SCHED_STATIC,
                       chunk_size,
                       &t_static);
    printf("STATIC done in %.6f seconds.\n", t_static);

    printf("Saving terrain_static.pgm...\n");
    if (write_pgm("terrain_static.pgm", heightmap_static, width, height) != 0) {
        fprintf(stderr, "Failed to write terrain_static.pgm\n");
    } else {
        printf("Saved terrain_static.pgm\n");
    }

    printf("\nGenerating terrain with DYNAMIC schedule...\n");
    generate_heightmap(heightmap_dynamic,
                       width,
                       height,
                       scale,
                       octaves,
                       2.0f,   /* lacunarity */
                       0.5f,   /* gain */
                       threads,
                       SCHED_DYNAMIC,
                       chunk_size,
                       &t_dynamic);
    printf("DYNAMIC done in %.6f seconds.\n", t_dynamic);

    printf("Saving terrain_dynamic.pgm...\n");
    if (write_pgm("terrain_dynamic.pgm", heightmap_dynamic, width, height) != 0) {
        fprintf(stderr, "Failed to write terrain_dynamic.pgm\n");
    } else {
        printf("Saved terrain_dynamic.pgm\n");
    }

    printf("\nSummary:\n");
    printf("  STATIC  : %.6f s\n", t_static);
    printf("  DYNAMIC : %.6f s\n", t_dynamic);
    if (t_dynamic > 0.0) {
        printf("  Speedup (STATIC/DYNAMIC): %.3fx\n", t_static / t_dynamic);
    }

    free(heightmap_static);
    free(heightmap_dynamic);

    return EXIT_SUCCESS;
}
