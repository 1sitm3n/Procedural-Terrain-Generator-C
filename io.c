/* io.c
 *
 * File I/O utilities for saving heightmaps.
 */

#include <stdio.h>
#include <stdlib.h>
#include "io.h"

int write_pgm(const char *filename,
              const float *heightmap,
              int width,
              int height) {

    if (!filename || !heightmap || width <= 0 || height <= 0) {
        return -1;
    }

    int size = width * height;
    float min = heightmap[0];
    float max = heightmap[0];

    for (int i = 1; i < size; ++i) {
        if (heightmap[i] < min) min = heightmap[i];
        if (heightmap[i] > max) max = heightmap[i];
    }

    float range = max - min;
    if (range == 0.0f) {
        range = 1.0f;
    }

    unsigned char *buffer = (unsigned char *)malloc((size_t)size);
    if (!buffer) {
        return -2;
    }

    for (int i = 0; i < size; ++i) {
        float norm = (heightmap[i] - min) / range;
        if (norm < 0.0f) norm = 0.0f;
        if (norm > 1.0f) norm = 1.0f;
        buffer[i] = (unsigned char)(norm * 255.0f + 0.5f);
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        free(buffer);
        return -3;
    }

    if (fprintf(fp, "P5\n%d %d\n255\n", width, height) < 0) {
        fclose(fp);
        free(buffer);
        return -4;
    }

    size_t written = fwrite(buffer, 1, (size_t)size, fp);
    fclose(fp);
    free(buffer);

    if (written != (size_t)size) {
        return -5;
    }

    return 0;
}
