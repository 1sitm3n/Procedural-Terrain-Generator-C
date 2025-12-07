/* terrain.h
 *
 * Terrain heightmap generation using Perlin noise and OpenMP.
 */

#ifndef TERRAIN_H
#define TERRAIN_H

typedef enum {
    SCHED_STATIC = 0,
    SCHED_DYNAMIC = 1
} schedule_mode_t;

/* Generates a heightmap of size width x height.
 * heightmap: preallocated array of length width * height.
 * scale:    frequency scaling for noise sampling.
 * octaves:  number of fBm octaves.
 * lacunarity: frequency multiplier between octaves.
 * gain:       amplitude multiplier between octaves.
 * num_threads: number of OpenMP threads.
 * mode:       static or dynamic scheduling.
 * chunk_size: OpenMP schedule chunk size.
 * out_time:   if not NULL, the elapsed time in seconds is stored here.
 */
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
                        double *out_time);

#endif /* TERRAIN_H */
