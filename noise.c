/* noise.c
 *
 * 2D noise implementations - classic Perlin, and simplex.
 * Both index the same permutation table, seeded by noise_init().
 */

#include <math.h>
#include <stdlib.h>
#include "noise.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int perm[512];
static int base_perm[256] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,
    140,36,103,30,69,142,8,99,37,240,
    21,10,23,190, 6,148,247,120,234,75,
    0,26,197,62,94,252,219,203,117,35,
    11,32,57,177,33,88,237,149,56,87,
    174,20,125,136,171,168, 68,175,74,
    165,71,134,139,48,27,166,77,146,158,
    231,83,111,229,122,60,211,133,230,
    220,105,92,41,55,46,245,40,244,102,
    143,54, 65,25,63,161, 1,216,80,73,
    209,76,132,187,208, 89,18,169,200,
    196,135,130,116,188,159,86,164,100,
    109,198,173,186, 3,64,52,217,226,
    250,124,123, 5,202,38,147,118,126,
    255,82,85,212,207,206,59,227,47,16,
    58,17,182,189,28,42,223,183,170,213,
    119,248,152, 2,44,154,163, 70,221,
    153,101,155,167, 43,172,9,129,22,
    39,253, 19,98,108,110,79,113,224,
    232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,
    179,162,241,81,51,145,235,249,14,
    239,107,49,192,214, 31,181,199,106,
    157,184, 84,204,176,115,121,50,45,
    127, 4,150,254,138,236,205,93,222,
    114,67,29,24,72,243,141,128,195,
    78,66,215,61,156,180
};

static float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static float grad(int hash, float x, float y) {
    int h = hash & 7;  /* Using 8 gradient directions */
    float u = (h < 4) ? x : y;
    float v = (h < 4) ? y : x;
    float res = ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    return res;
}

void noise_init(unsigned int seed) {
    int i;
    for (i = 0; i < 256; ++i) {
        perm[i] = base_perm[i];
    }

    if (seed != 0) {
        srand(seed);
        for (i = 255; i > 0; --i) {
            int j = rand() % (i + 1);
            int tmp = perm[i];
            perm[i] = perm[j];
            perm[j] = tmp;
        }
    }

    for (i = 0; i < 256; ++i) {
        perm[256 + i] = perm[i];
    }
}

float perlin2d(float x, float y) {
    int X = (int)floorf(x) & 255;
    int Y = (int)floorf(y) & 255;

    float xf = x - floorf(x);
    float yf = y - floorf(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = perm[perm[X]     + Y];
    int ab = perm[perm[X]     + Y + 1];
    int ba = perm[perm[X + 1] + Y];
    int bb = perm[perm[X + 1] + Y + 1];

    float x1 = lerp(grad(aa, xf,     yf),
                    grad(ba, xf - 1, yf), u);
    float x2 = lerp(grad(ab, xf,     yf - 1),
                    grad(bb, xf - 1, yf - 1), u);

    float result = lerp(x1, x2, v);

    return result;
}


/* ─── Simplex noise ───────────────────────────────────────────────────────
 *
 * Ken Perlin, "Improving Noise" (SIGGRAPH 2002). The skew constants are the
 * 2D case of the general form: F = (sqrt(n+1) - 1) / n and
 * G = (1 - 1/sqrt(n+1)) / n, which at n=2 give the values below.
 *
 * The idea: skew the input point into a space where the simplex grid becomes
 * a square grid, find which of the two triangles in that square the point
 * landed in, then unskew the three corners back and sum their contributions.
 * Three corners rather than Perlin's four, and no axis-aligned lattice to
 * leave directional artefacts in the output.
 */

#define SIMPLEX_F2 0.3660254037844386f   /* (sqrt(3) - 1) / 2 */
#define SIMPLEX_G2 0.2113248654051871f   /* (3 - sqrt(3)) / 6 */

/* Gradient set for simplex: the 8 compass directions. grad() above is the
   Perlin variant and picks from a different set, so this is separate. */
static float sgrad(int hash, float x, float y) {
    switch (hash & 7) {
        case 0: return  x + y;
        case 1: return  x - y;
        case 2: return -x + y;
        case 3: return -x - y;
        case 4: return  x;
        case 5: return -x;
        case 6: return  y;
        default: return -y;
    }
}

float simplex2d(float x, float y) {
    /* Skew into the lattice where simplices become unit squares. */
    const float s = (x + y) * SIMPLEX_F2;
    const int   i = (int)floorf(x + s);
    const int   j = (int)floorf(y + s);

    /* Unskew the cell origin back and take the offset from it. */
    const float t   = (float)(i + j) * SIMPLEX_G2;
    const float x0  = x - ((float)i - t);
    const float y0  = y - ((float)j - t);

    /* Which of the two triangles: lower (1,0) or upper (0,1). */
    const int i1 = (x0 > y0) ? 1 : 0;
    const int j1 = (x0 > y0) ? 0 : 1;

    /* The other two corners, in unskewed space. */
    const float x1 = x0 - (float)i1 + SIMPLEX_G2;
    const float y1 = y0 - (float)j1 + SIMPLEX_G2;
    const float x2 = x0 - 1.0f + 2.0f * SIMPLEX_G2;
    const float y2 = y0 - 1.0f + 2.0f * SIMPLEX_G2;

    const int ii = i & 255;
    const int jj = j & 255;

    float n = 0.0f;

    /* Each corner contributes (r^2 - d^2)^4 * gradient-dot-offset, clamped at
       zero so a corner outside the radius of influence drops out entirely.
       That is what keeps it continuous without needing Perlin's fade curve. */
    float t0 = 0.5f - x0*x0 - y0*y0;
    if (t0 > 0.0f) { t0 *= t0; n += t0 * t0 * sgrad(perm[ii + perm[jj]], x0, y0); }

    float t1 = 0.5f - x1*x1 - y1*y1;
    if (t1 > 0.0f) { t1 *= t1; n += t1 * t1 * sgrad(perm[ii + i1 + perm[jj + j1]], x1, y1); }

    float t2 = 0.5f - x2*x2 - y2*y2;
    if (t2 > 0.0f) { t2 *= t2; n += t2 * t2 * sgrad(perm[ii + 1 + perm[jj + 1]], x2, y2); }

    /* 70 is the standard scale factor that brings the sum into [-1, 1]. */
    return 70.0f * n;
}

float noise2d(noise_type_t type, float x, float y) {
    return (type == NOISE_SIMPLEX) ? simplex2d(x, y) : perlin2d(x, y);
}
