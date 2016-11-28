#ifndef __BLISS_B_PARAMS__
#define __BLISS_B_PARAMS__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int32_t q;                          // field modulus
    int32_t n;                          // ring size (x^n+1)
    int32_t d;                          // bit drop shift
    int32_t p;                          // magic modulus
    int32_t kappa;                      // index vector size
    size_t  theta;                      // cseed in bytes
    int32_t b_inf;                      // infinite norm
    int32_t b_l2;                       // L2 norm
    int32_t nz1;                        // nonzero +-1
    int32_t nz2;                        // nonzero +-2
    int32_t pmax;                       // derived from nt, nz2, n, kappa
    long double sigma;                  // standard deviation
    long double m;                      // repetition rate
    const int32_t *w;                   // n roots of unity (mod q)
    const int32_t *r;                   // w[i]/n (mod q)
} bliss_param_t;



/* The five varieties of bliss-b */
extern const bliss_param_t bliss_param[]; 











#endif
