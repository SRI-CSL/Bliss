#include <stdlib.h>
#include <mpfr.h>

#include "sampler.h"
#include "tables.h"



bool sampler_init(sampler_t *sampler, uint32_t sigma, uint32_t ell, uint32_t precision){
  if(sampler != NULL){
    sampler->sigma = sigma;
    sampler->ell = ell;
    sampler->precision = precision;
    sampler->columns = sampler->precision / 8;
    sampler->c = get_table(sigma, ell, precision);
    if(sampler->c != NULL){
      sampler->k_sigma = get_k_sigma(sigma, precision);
      sampler->k_sigma_bits = get_k_sigma_bits(sigma, precision);
    }
    return sampler->c != NULL;
  }
  return false;
}

void sampler_delete(sampler_t *sampler){
  if(sampler != NULL){
    entropy_delete(&sampler->entropy);
  }
}


/* Sampling Bernoulli_c with c a constant in [0, 1] */
bool sampler_ber(sampler_t* sampler, const uint8_t* p, bool* accepted){
  if(sampler != NULL && p != NULL && accepted != NULL){
    uint8_t i, uc;
    for(i = 0; i < sampler->columns; i++){
      if(!entropy_random_uint8(&sampler->entropy, &uc)){ return false; }
      if(uc < *p){ *accepted = true; return true; }
      if(uc > *p){ *accepted = false; return true; }
      p++;
    }
    *accepted = true;
    return true;
  }
  return false;
}

/* Sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma); Algorithm 8 from DDLL */
bool sampler_ber_exp(sampler_t* sampler, uint32_t x, bool* accepted){
  if(sampler != NULL && accepted != NULL){
    const uint8_t* row;
    uint32_t mask;
    uint32_t ri;
    bool saccptd;
    ri = sampler->ell - 1;
    mask = 1 << ri;
    row = sampler->c + (ri * sampler->columns);
    while (mask > 0){
      if(x & mask){
	if(!sampler_ber(sampler, row, &saccptd)){ return false; }
	if(!saccptd){ *accepted = true; return true; }
      }
      mask >>= 1;
      row -= sampler->columns;
    }
    return true;
  }
  return false;
}

/* Sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma) */
bool sampler_ber_cosh(sampler_t* sampler, int32_t x, bool* accepted){
  bool rbit;
  if(sampler != NULL && accepted != NULL){
    x = x < 0 ? -x : x;
    x <<= 1;
    while(true){
      if(!sampler_ber_exp(sampler, x, accepted)){ return false; }
      if(*accepted){ return true; }
      if(!entropy_random_bit(&sampler->entropy, &rbit)){ return false; }
      if(!rbit){
	if(!sampler_ber_exp(sampler, x, accepted)){ return false; }
	if(!(*accepted)){ return true; }
      }
    }
    return true;
  }
  return false;
}

/* Sample the sign according to the binary distribution  */

static const int32_t max_sample_count  = 16;

extern bool sampler_pos_binary(sampler_t* sampler, int32_t* valp){
  if(sampler != NULL && valp != NULL){
    uint32_t u, i;
    while(true){
      
      for(i = 0; i <= max_sample_count; i++){
	if(!entropy_random_bits(&sampler->entropy,  i ? (2*i - 1) : 1, &u)){ return false; }
	if(u == 0){ *valp = i; return true; }
	if((u >> 1) != 0){  break; }
      }

      if(i >  max_sample_count){
	return false;
      }
    }
  }
  return false;
}



/* 
 * Sampling the Gaussian distribution exp(-x^2/(2*sigma*sigma))
 *
 *   returns true is the sampling was successful, false if something went wrong
 *
 *   accepted will  point to true in the value was accepted, false if it was rejected.
 *
 */
extern bool sampler_ber_gauss(sampler_t* sampler, int32_t val, bool* accepted){
  if(sampler != NULL && accepted != NULL){



    return true;
  }
  return false;
}
