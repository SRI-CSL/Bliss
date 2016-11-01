#ifndef _SAMPLER_H
#define _SAMPLER_H


#include "entropy.h"


typedef struct sampler_s {
  entropy_t entropy;     /* BD thinks we should use a function pointer; rather than hard code
                          *  this sepcific implementation. Seems reasonable. TODO.
                          */
  const uint8_t *c;      /* the table we will use (from tables.h) */
  uint32_t sigma;        /* the standard deviation of the distribution */
  uint32_t ell;          /* rows in the table     */
  uint32_t precision;    /* precision used in computing the tables */
  uint32_t columns;      /* columns = precision/8 */
  uint16_t k_sigma;      /* k_sigma = ceiling[ sqrt(2*ln 2) * sigma ]  */
  uint16_t k_sigma_bits; /* number of significant bits in k_sigma */
} sampler_t;


/*
 *    Initialize a sampler:
 *
 *       sigma: the standard deviation
 *          ell: the number of significant bits (i.e. the number of rows in the table.h)
 *    precision: the precision, (i.e. precision = 8 * the number of columns in the table.h)
 *
 *    See table.h for the currently accepted values.
 *
 */
extern bool sampler_init(sampler_t* sampler, uint32_t sigma, uint32_t ell, uint32_t precision);

extern void sampler_delete(sampler_t* sampler);

/* 
 * Sampling Bernoulli_p with p a constant in [0, 1] TODO add the length of val ? 
 *
 *   p is represented as an array of bytes, val.
 *
 *   returns true is the sampling was successful, false if something went wrong
 *
 *   accepted will  point to true in the value was accepted, false if it was rejected.
 */
extern bool sampler_ber(sampler_t* sampler, const uint8_t* val, bool* accepted);

/* 
 * Sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma)
 *
 *   returns true is the sampling was successful, false if something went wrong
 *
 *   accepted will  point to true in the value was accepted, false if it was rejected.
 *
 */
extern bool sampler_ber_exp(sampler_t* sampler, uint32_t val, bool* accepted);

/* Sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma)
 *
 *   returns true is the sampling was successful, false if something went wrong
 *
 *   accepted will  point to true in the value was accepted, false if it was rejected.
 *
 */
extern bool sampler_ber_cosh(sampler_t* sampler, int32_t val, bool* accepted);



extern bool sampler_pos_binary(sampler_t* sampler, uint32_t* valp);



/* 
 * Sampling the Gaussian distribution exp(-x^2/(2*sigma*sigma))
 *
 *   returns true is the sampling was successful, false if something went wrong
 *
 *   If successful, valp will point to the generated value.
 *
 */
extern bool sampler_ber_gauss(sampler_t* sampler, int32_t *valp);




#endif
