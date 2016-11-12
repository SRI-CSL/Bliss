#ifndef __SAMPLER_H
#define __SAMPLER_H

#include <stdint.h>
#include <stdbool.h>

#include "entropy.h"

typedef struct sampler_s {
  entropy_t entropy;
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
 *
 * - sigma: the standard deviation
 * - ell: the number of significant bits (i.e. the number of rows in the table.h)
 * - precision: the precision, (i.e. precision = 8 * the number of columns in the table.h)
 * - seed: array of SHA3_512_DIGEST_SIZE bytes (i.e., 64 bytes)
 *
 * Returns false if the combination sigma/ell/precision is not supported.
 * See table.h for the currently accepted values.
 *
 * Returns true otherwise.
 */
extern bool sampler_init(sampler_t *sampler, uint32_t sigma, uint32_t ell, uint32_t precision,
			 const uint8_t *seed);


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
 * Sample an integer according to the distribution 2^(-x^2)
 * - return true if this succeeds, false otherwise
 * - the sampled integer is returned in *x
 */
extern bool sampler_pos_binary(sampler_t *sampler, uint32_t *x);



/* 
 * Sampling the Gaussian distribution exp(-x^2/(2*sigma*sigma))
 *
 *   returns true is the sampling was successful, false if something went wrong
 *
 *   If successful, valp will point to the generated value.
 *
 */
extern bool sampler_gauss(sampler_t *sampler, int32_t *valp);


#endif
