/* noise.h
 *
 * 2D value noise: classic Perlin and simplex.
 *
 * Both share the same permutation table, so noise_init() seeds both.
 */

#ifndef NOISE_H
#define NOISE_H

typedef enum {
    NOISE_PERLIN  = 0,
    NOISE_SIMPLEX = 1
} noise_type_t;

void  noise_init(unsigned int seed);
float perlin2d(float x, float y);

/* Simplex noise, Perlin's 2001 successor to the lattice version above.
 *
 * Worth having both rather than picking one: they are the same idea with a
 * different lattice. Perlin interpolates across a square grid, so it has to
 * blend four corners and the axis-aligned grid shows up as directional
 * artefacts in the output. Simplex skews the input onto a triangular lattice,
 * which needs three corners instead of four, has no preferred direction, and
 * scales as O(n^2) rather than O(2^n) in dimension - irrelevant at 2D, the
 * reason it exists at 3D and above.
 *
 * Range is normalised to roughly [-1, 1] to match perlin2d(), so the fBm code
 * above it does not care which one it is calling. */
float simplex2d(float x, float y);

/* Dispatch by type, for the --noise flag. */
float noise2d(noise_type_t type, float x, float y);

#endif /* NOISE_H */
