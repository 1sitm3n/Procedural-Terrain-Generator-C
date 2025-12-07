/* io.h
 *
 * File I/O utilities for saving heightmaps.
 */

#ifndef IO_H
#define IO_H

int write_pgm(const char *filename,
              const float *heightmap,
              int width,
              int height);

#endif /* IO_H */
