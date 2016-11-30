// ntt32.c
// 09-Jun-15  Markku-Juhani O. Saarinen <m.saarinen@qub.ac.uk>

// Number-Theoretic Transforms on a max 31-bit q value

#include <stdlib.h>
#include "ntt_blzzd.h"

// Basic arithmetic primitives
// Written as separate functions -- so that they can be instrumented easily.

static int32_t ntt32_muln(int32_t x, int32_t y, int32_t n)
{
    return (((int64_t) x) * ((int64_t) y)) % ((int64_t) n);
}

static int32_t ntt32_sqrn(int32_t x, int32_t n)
{
    return (((int64_t) x) * ((int64_t) x)) % ((int64_t) n);
}

// Compute x^e (mod n).

int32_t ntt32_pwr(int32_t x, int32_t e, int32_t n)
{
    int32_t y;

    y = 1;
    if (e & 1)
        y = x;

    e >>= 1;

    while (e > 0) {
        x = ntt32_sqrn(x, n);
        if (e & 1)
            y = ntt32_muln(x, y, n);
        e >>= 1;
    }

    return y;
}

// Find NTT roots of unity.
//  Return w[1] or 0 on failure (g is not a generator).

int32_t ntt32_wgn(int32_t w[], size_t n, int32_t q, int32_t g)
{
    size_t i;
    int32_t x, y;

    // reduce order to 2n
    x = ntt32_pwr(g, (q - 1) / (n << 1), q);

    y = x;                              // test order via repeated squaring
    for (i = 1; i < 2 * n; i <<= 1) {
        y = ntt32_sqrn(y, q);
        if (y == 1)
            break;
    }
    if (i != n)
        return 0;                       // return 0 on failure

    w[0] = 1;
    w[1] = x;
    y = x;

    for (i = 2; i < n; i++) {
        y = ntt32_muln(y, x, q);
        w[i] = y;
    }

    return x;
}

// FFT operation (forward and inverse).

void ntt32_fft(int32_t v[], size_t n, int32_t q, const int32_t w[])
{
    size_t i, j, k, l;
    int32_t x, y;

    // bit-inverse shuffle
    j = n >> 1;
    for (i = 1; i < n - 1; i++) {       // 00..0 and 11..1 remain same
        if (i < j) {
            x = v[i];
            v[i] = v[j];
            v[j] = x;
        }
        k = n;
        do {
            k >>= 1;
            j ^= k;
        } while ((j & k) == 0);
    }

    // main loops
    for (i = 1; i < n; i <<= 1) {

        l = n / i;

        for (k = 0; k < n; k += i + i) {
            x = v[k + i];
            v[k + i] = v[k] - x;
            v[k] = v[k] + x;
        }

        for (j = 1; j < i; j++) {
            y = w[j * l];
            for (k = j; k < n; k += i + i) {
                x = ntt32_muln(v[k + i], y, q);
                v[k + i] = v[k] - x;
                v[k] = v[k] + x;
            }
        }
    }
}

// Elementvise vector product  v = t (*) u.

void ntt32_xmu(int32_t v[], size_t n, int32_t q,
    const int32_t t[], const int32_t u[])
{
    size_t i;

    // multiply each element point-by-point
    for (i = 0; i < n; i++) {
        v[i] = ntt32_muln(t[i], u[i], q);
    }
}

// Multiply with a scalar  v = t * c.

void ntt32_cmu(int32_t v[], size_t n, int32_t q,
    const int32_t t[], int32_t c)
{
    size_t i;
    int32_t x;

    for (i = 0; i < n; i++) {
        x = ntt32_muln(t[i], c, q);
        if (x < 0)
            x += q;
        v[i] = x;
    }
}

// Flip the order.

void ntt32_flp(int32_t v[], size_t n, int32_t q)
{
    size_t i, j;
    int32_t x;

    for (i = 1, j = n - 1; i < j; i++, j--) {
        x = v[i];
        v[i] = v[j];
        v[j] = x;
    }
    v[0] = -v[0];

    for (i = 0; i < n; i++) {
        x = v[i];
        if (x < 0)
            x += q;
        if (x >= q)
            x -= q;
        v[i] = x;
    }
}

