#include <stdlib.h>
#include <mpfr.h>

#include "sampler.h"

static void sampler_init_k(sampler_t *sampler){

  sampler->k = (uint64_t) (sigma_bin_inv_lowprec * sampler->stddev) + 1;


}

static void sampler_init_mask(sampler_t *sampler){


}

static void sampler_init_f(sampler_t *sampler, mpfr_t f){


}




static void sampler_init_c(uint32_t ell, uint8_t *c, mpfr_t f){
  int32_t i, j;
  mpfr_t y, t, z;
  mpfr_init2(t, 128);
  mpfr_init2(z, 128);
  mpfr_init2(y, 128);

  
  mpfr_set_ui(t, 8, GMP_RNDN);
  /* t = 2^8 */
  mpfr_exp2(t, t, GMP_RNDN);


  mpfr_set_ui(y, 1, GMP_RNDN);
 
  for(i = 0; i < ell; i++){

    /*  z = 2^i */
    mpfr_set(z, y, GMP_RNDN); 

    /*  z = -2^i */
    mpfr_mul_si(z, z, -1, GMP_RNDN);

    /*  z = -2^i/f      */
    mpfr_div(z, z, f, GMP_RNDN); 

    /*  z = exp(-2^i/f) */
    mpfr_exp(z, z, GMP_RNDN); 

    for (j=0; j<16; j++) {
      /*  z = exp(-2^i/f)*2^8  */
      mpfr_mul(z, z, t, GMP_RNDN); 
      
      c[i*16+j] = (uint8_t)mpfr_get_ui(z, GMP_RNDD);

      /* ? */
      mpfr_sub_ui(z, z, (uint64_t) c[i*16+j], GMP_RNDN);

    }

    /* y = 2 * y */
    mpfr_mul_ui(y,y, 2, GMP_RNDN); 
    
  }
}




bool sampler_init(sampler_t *sampler, int64_t stddev, double alpha){
  if(sampler != NULL){
    mpfr_t f;
    mpfr_init2(f, 128);

    sampler->stddev = stddev;

    sampler->alpha = alpha;

    sampler_init_k(sampler);

    sampler_init_mask(sampler);

    sampler_init_f(sampler, f);
    
    sampler->ell = 64;  /* need to grok the obscure calculation of this */

    sampler->c = malloc(16 * sampler->ell * sizeof(uint8_t));
    if(sampler->c !=  NULL){ 
      sampler_init_c(sampler->ell, sampler->c, f);

      return entropy_init(&sampler->entropy);
    }
    mpfr_clear(f);
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
bool sampler_ber(sampler_t* sampler, const uint8_t* p){
  if(sampler != NULL && p != NULL){
    uint8_t i, uc;
    for(i = 0; i < 16; i++){
      uc = entropy_random_uint8(&sampler->entropy);
      if(uc < *p){ return true; }
      if(uc > *p){ return false; }
      p++;  //iam: huh?
    }
  }
  return true;
}

/* sampling Bernoulli_E with E = exp(-x/(2*sigma*sigma) */
bool sampler_ber_exp(sampler_t* sampler, uint64_t x){



  return true;
}

/* sampling Bernoulli_C with C = 1/cosh(x/(sigma*sigma) */
bool sampler_ber_cosh(sampler_t* sampler, int64_t x){


  return true;
}
