#ifndef __SAMPLER_H
#define __SAMPLER_H

// Feb 10, 2017: changed data structure so that a sampler
// has a pointer to an entropy object.

#include <stdint.h>
#include <stdbool.h>

#include "entropy.h"
#include "bliss_b_params.h"

typedef struct sampler_s {
  entropy_t *entropy;
  const uint8_t *c;      /* the table we will use (from tables.h) */
  uint32_t sigma;        /* the standard deviation of the distribution */
  uint32_t ell;          /* rows in the table     */
  uint32_t precision;    /* precision used in computing the tables */
  uint32_t columns;      /* columns = precision/8 */
  uint16_t k_sigma;      /* k_sigma = ceiling[ sqrt(2*ln 2) * sigma ]  */
  uint16_t k_sigma_bits; /* number of significant bits in k_sigma */
} sampler_t;


/*
 * Initialize a sampler:
 * - params = Bliss b parameters 
 * - the entropy object to use.
 *
 * The sampler uses the following fields of the params record:
 * - sigma: the standard deviation
 * - ell: the number of significant bits (i.e. the number of rows in the table.h)
 * - precision: the precision, (i.e. precision = 8 * the number of columns in the table.h)
 *
 */
extern void sampler_init(sampler_t *sampler, const bliss_b_params_t *params, entropy_t *entropy);


/* 
 * Sampling Bernoulli_p with p a constant in [0, 1]
 *
 * - p is represented as an array of bytes val. 
 * - It must have the same number of bits as the sampler's precision.
 * - the constant is encoded using big-endian format
 *
 * - returns true with probability p, false with probability 1-p
 */
extern bool sampler_ber(sampler_t *sampler, const uint8_t *val);

/* 
 * Sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma)
 *
 * - returns true with probability E
 * - returns false with probability 1-E
 */
extern bool sampler_ber_exp(sampler_t *sampler, uint32_t x);

/*
 * Sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma)
 *
 * - returns true with probability C
 * - returns false with probability 1-C 
 */
extern bool sampler_ber_cosh(sampler_t *sampler, int32_t x);

/*
 * Sample a non-negative integer according to the probability density 
 * p(x) = 2^(-x^2)
 * - return true if this succeeds, false otherwise
 * - the sampled integer is returned in *x
 */
bextern bool sampler_pos_binary(sampler_t *sampler, uint32_t *x);

/*
 * Variant implementation: return the sampled integer.
 * Doesn't fail.
 */
extern uint32_t sampler_pos_binary2(sampler_t *sample);

/* 
 * Sampling the Gaussian distribution exp(-x^2/(2*sigma*sigma))
 *
 * returns true is the sampling was successful, false if something went wrong
 *
 * If successful, the generated value is returned in *valp.
 */
extern bool sampler_gauss(sampler_t *sampler, int32_t *valp);

/*
 * Variant implementation: returns the sampled value.
 * Does not fail.
 */
extern int32_t sampler_gauss2(sampler_t *sampler);


#endif
