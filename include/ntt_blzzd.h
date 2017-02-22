// ntt32.h
// 06-May-15  Markku-Juhani O. Saarinen <m.saarinen@qub.ac.uk>

#ifndef NTT32_H
#define NTT32_H

#include <stdint.h>
#include <stddef.h>

// FFT operation (forward and inverse).
void ntt32_fft(int32_t v[], uint32_t n, int32_t q, const int32_t w[]);

// Flip the order after inverse FFT.
void ntt32_flp(int32_t v[], uint32_t n, int32_t q);

// Elementvise vector product  v = t (*) u
void ntt32_xmu(int32_t v[], uint32_t n, int32_t q, const int32_t t[], const int32_t u[]);

// Multiply vector with a scalar  v = v * c
void ntt32_cmu(int32_t v[], uint32_t n, int32_t q, const int32_t t[], int32_t c);

// Compute x^n (mod q).
int32_t ntt32_pwr(int32_t x, int32_t n, int32_t q);

#endif

