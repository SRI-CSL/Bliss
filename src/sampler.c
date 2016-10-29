#include <stdlib.h>
#include <mpfr.h>

#include "sampler.h"
#include "tables.h"



bool sampler_init(sampler_t *sampler, uint32_t stddev, uint32_t ell, uint32_t precision){
  if(sampler != NULL){
    sampler->stddev = stddev;
    sampler->ell = ell;
    sampler->precision = precision;
  }
  return false;
}

void sampler_delete(sampler_t *sampler){
  if(sampler != NULL){
    free(sampler->c);
    entropy_delete(&sampler->entropy);
  }
}



/* sampling Bernoulli_c with c a constant in [0, 1] */
bool sampler_ber(sampler_t* sampler, const uint8_t* p, bool* accepted){
  if(sampler != NULL && p != NULL && accepted != NULL){
    uint8_t i, uc;
    for(i = 0; i < 16; i++){
      uc = entropy_random_uint8(&sampler->entropy);
      if(uc < *p){ *accepted = true; return true; }
      if(uc > *p){ *accepted = false; return true; }
      p++;  //iam: huh?
    }
  }
  return false;
}

/* sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma) */
bool sampler_ber_exp(sampler_t* sampler, uint64_t x, bool* accepted){
  if(sampler != NULL && accepted != NULL){

    return true;
  }
  return false;
}

/* sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma) */
bool sampler_ber_cosh(sampler_t* sampler, int64_t x, bool* accepted){
  if(sampler != NULL && accepted != NULL){

    return true;
  }
  return false;
}
