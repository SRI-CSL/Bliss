#include <assert.h>
#include <stdlib.h>

#include "sampler.h"
#include "tables.h"

/*
 * Initialize sampler:
 * - return true if success/false if error
 * - false means that the parameters sigma/ell/precisions are not supported
 */
bool sampler_init(sampler_t *sampler, uint32_t sigma, uint32_t ell, uint32_t precision, entropy_t *entropy) {
  sampler->entropy = entropy;
  sampler->sigma = sigma;
  sampler->ell = ell;
  sampler->precision = precision;
  sampler->columns = sampler->precision / 8;
  sampler->c = get_table(sigma, ell, precision);
  if (sampler->c != NULL) {
    sampler->k_sigma = get_k_sigma(sigma, precision);
    sampler->k_sigma_bits = get_k_sigma_bits(sigma, precision);
    return true;
  }
  return false;
}


/*
 * Sampling Bernoulli_c with c a constant in [0, 1]
 * - p = array of bytes (encoding c in big-endian format)
 * - p must have as many bytes as sampler->columns (= precision/8)
 */
bool sampler_ber(sampler_t *sampler, const uint8_t *p) {
  uint32_t i;
  uint8_t uc;

  assert(sampler != NULL && p != NULL);

  for(i = 0; i < sampler->columns; i++) {
    uc = entropy_random_uint8(sampler->entropy);
    if (uc < p[i]) return true;
    if (uc > p[i]) return false;
  }
  return true; // default value
}

/*
 * Sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma)).
 * Algorithm 8 from DDLL
 */
bool sampler_ber_exp(sampler_t* sampler, uint32_t x) {
  const uint8_t* row;
  uint32_t mask;
  uint32_t ri;
  bool bit;

  ri = sampler->ell - 1;
  mask = 1u << ri;
  row = sampler->c + (ri * sampler->columns);
  while (mask > 0) {
    if (x & mask) {
      bit = sampler_ber(sampler, row);
      if(!bit) return false;
    }
    mask >>= 1;
    row -= sampler->columns;
  }

  return true;
}

#if 0

// NOT USED YET

/*
 * Sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma))
 * in constant time.
 */
bool sampler_ber_exp_ct(sampler_t* sampler, uint32_t x) {
  const uint8_t* row;
  uint32_t xi;
  uint32_t i;
  bool bit;
  uint32_t ret = 1;

  xi = x;
  row = sampler->c;
  for ( i = sampler->ell - 1; i!=0; i--) {
    bit = sampler_ber(sampler, row);
    ret = ret * (1-(xi&1)+bit*(xi&1));
    xi >>= 1;
    row += sampler->columns;
  }

  return (bool) ret;
}

#endif


/*
 * Sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma)) 
 */
bool sampler_ber_cosh(sampler_t* sampler, int32_t x) {
  bool bit;

  // How do we know this does not overflow/underflow?
  x = x < 0 ? -x : x;
  x <<= 1;

  while (true) {
    bit = sampler_ber_exp(sampler, (uint32_t)x);
    if (bit) return true;

    bit = entropy_random_bit(sampler->entropy);
    if (!bit) {
      bit = sampler_ber_exp(sampler, (uint32_t)x);
      if (!bit) return false;
    }
  }
}


/*
 * Sample a non-negative integer according to the binary discrete
 * Gaussian distribution.
 *
 * Algorithm 10 in DDLL.
 */

#define MAX_SAMPLE_COUNT 16

uint32_t sampler_pos_binary(sampler_t *sampler) {
  uint32_t u, i;

 restart:
  if (entropy_random_bit(sampler->entropy)) {
    return 0;
  }

  for (i=1; i <= MAX_SAMPLE_COUNT; i++) {
    u = entropy_random_bits(sampler->entropy, 2*i - 1);
    if (u == 0) {
      return i;
    }
    if (u != 1) {
      goto restart;
    }
  }
  return 0; // default value. Extremely unlikely to ever be reached
}


/* 
 * Sampling the Gaussian distribution exp(-x^2/(2*sigma*sigma))
 *
 * returns the sampled value.
 *
 * Combination of Algorithms 11 and 12 from DDLL.
 */
int32_t sampler_gauss(sampler_t *sampler) {
  uint32_t u, e, x, y;
  int32_t val_pos;

  while (true) {
    x = sampler_pos_binary(sampler);

    do {
      y = entropy_random_bits(sampler->entropy, sampler->k_sigma_bits);
    } while (y >= sampler->k_sigma);

    e = y * (y + 2u * sampler->k_sigma * x);
    u = entropy_random_bit(sampler->entropy);
    // don't restart if both hold:
    // 1. (x, y) != (0, 0) or u = 1
    // 2. sampler_ber_exp(sampler, e) = 1
    if (x | y | u) {
      if (sampler_ber_exp(sampler, e)) break; // lazy sample
    }
  }

  val_pos = (int32_t)(sampler->k_sigma * x + y);
  return u ? val_pos : - val_pos;
}


#if 0

// TO BE DONE

/* 
 * Sampling the Gaussian distribution exp(-x^2/(2*sigma*sigma)) using a cumulative distribution table
 *
 * returns the sampled value.
 */
int32_t sampler_gauss_CDT(sampler_t *sampler) {
  assert(0);
  return 0;
}

#endif
