#ifndef _SAMPLER_H
#define _SAMPLER_H


#include "entropy.h"


/* Gaussian "tail-cut" factor */
const int tau = 12;

/*  sqrt(2 ln 2)     */
const double sigma_bin_inv_lowprec = 1.17741002251547469101156932645969963;

/*  sqrt(1/(2 ln 2)) */
const double sigma_bin_lowprec = 0.8493218002880190427215028341028896; 


typedef struct sampler_s {
  entropy_t entropy;
  int64_t stddev;
  double alpha;

  uint64_t hack;   /* called k in Sampler.[h.cpp]                     */

  uint64_t mask;   /* called mask in Sampler.[h.cpp]                  */
  
  uint32_t ell;    /* appears to be the "l" in algorithm 8            */
  uint8_t* c;      /* appears to be the precomputation in algorithm 8 */
  
} sampler_t;


extern bool sampler_init(sampler_t*, int64_t, double);

extern void sampler_delete(sampler_t*);

/* sampling Bernoulli_p with p a constant in [0, 1] */
extern bool sampler_ber(sampler_t*, const uint8_t*);

/* sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma) */
extern bool sampler_ber_exp(sampler_t*, uint64_t);

/* sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma) */
extern bool sampler_ber_cosh(sampler_t*, int64_t);




#endif
