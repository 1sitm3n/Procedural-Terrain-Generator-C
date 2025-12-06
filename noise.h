/* noise.h
 *
 * 2D Perlin noise interface.
 */

#ifndef NOISE_H
#define NOISE_H

void noise_init(unsigned int seed);
float perlin2d(float x, float y);

#endif /* NOISE_H */
