#ifndef _POLYNOMIAL_H
#define _POLYNOMIAL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "entropy.h"



/*
   Constructs a random polyomial

   - v: where the random polynomial is stored
   - n: the length of the polynomial
   - nz1: the number of coefficients that are +-1
   - nz2: the number of coefficients that are +-2
   - zero: whether v should be zeroed out first, if false, v is assumed to be zeroed out.
   - entropy: an initialized source of randomness

*/
extern void uniform_poly(int32_t v[], int n, int nz1, int nz2, bool zero, entropy_t *entropy);


extern void fprint_poly(FILE*, int32_t v[], int32_t n);

#endif
