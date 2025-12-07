/* terrain.c
 *
 * Parallel terrain heightmap generation using Perlin-based fBm.
 */

#include <math.h>
#include <omp.h>
#include "terrain.h"
#include "noise.h"

static float fbm2d(float x, float y, int octaves, float lacunarity, float gain) {
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float sum = 0.0f;
    float max_sum = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        sum += perlin2d(x * frequency, y * frequency) * amplitude;
        max_sum += amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    if (max_sum > 0.0f) {
        sum /= max_sum;
    }

    sum = sum * 0.5f + 0.5f;  /* map roughly [-1,1] to [0,1] */

    if (sum < 0.0f) sum = 0.0f;
    if (sum > 1.0f) sum = 1.0f;

    return sum;
}

void generate_heightmap(float *heightmap,
                        int width,
                        int height,
                        float scale,
                        int octaves,
                        float lacunarity,
                        float gain,
                        int num_threads,
                        schedule_mode_t mode,
                        int chunk_size,
                        double *out_time) {

    if (scale <= 0.0f) {
        scale = 1.0f;
    }
    if (octaves < 1) {
        octaves = 1;
    }
    if (chunk_size <= 0) {
        chunk_size = 1;
    }
    if (num_threads <= 0) {
        num_threads = 1;
    }

    omp_set_num_threads(num_threads);

    omp_sched_t kind = (mode == SCHED_STATIC) ? omp_sched_static : omp_sched_dynamic;
    omp_set_schedule(kind, chunk_size);

    double start = omp_get_wtime();

    #pragma omp parallel for schedule(runtime)
    for (int y = 0; y < height; ++y) {
        float fy = (float)y / (float)height;
        for (int x = 0; x < width; ++x) {
            float fx = (float)x / (float)width;

            float sx = fx * scale;
            float sy = fy * scale;

            float value = fbm2d(sx, sy, octaves, lacunarity, gain);
            heightmap[y * width + x] = value;
        }
    }

    double end = omp_get_wtime();

    if (out_time != NULL) {
        *out_time = end - start;
    }
}
