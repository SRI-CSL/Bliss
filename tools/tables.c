#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <mpfr.h>


static void init_c(unsigned long int sigma, uint32_t ell, uint8_t *c, mpfr_t f, uint64_t precision){
  int32_t i, j;
  int32_t columns = precision / 8;
  mpfr_t y, t, z;
  uint8_t byte;

  mpfr_init2(t, precision);
  mpfr_init2(z, precision);
  mpfr_init2(y, precision);

  
  mpfr_set_ui(t, 8, GMP_RNDN);
  /* t = 2^8 */
  mpfr_exp2(t, t, GMP_RNDN);


  mpfr_set_ui(y, 1, GMP_RNDN);

  fprintf(stderr, "static const uint8_t c_bliss_%d_%d_%d[] = {\n", (int)sigma, ell, (int)precision);

 
  for(i = 0; i < ell; i++){

    /*  z = 2^i */
    mpfr_set(z, y, GMP_RNDN); 

    /*  z = -2^i */
    mpfr_mul_si(z, z, -1, GMP_RNDN);

    /*  z = -2^i/f      */
    mpfr_div(z, z, f, GMP_RNDN); 

    /*  z = exp(-2^i/f) */
    mpfr_exp(z, z, GMP_RNDN); 

    fprintf(stderr, "\t");

    for (j = 0; j < columns; j++) {
      /*  z = exp(-2^i/f)*2^8  */
      mpfr_mul(z, z, t, GMP_RNDN); 

      byte = (uint8_t)mpfr_get_ui(z, GMP_RNDD);
      c[i*16+j] = byte;
      fprintf(stderr, "%3d, ", byte);
	

      /* ? */
      mpfr_sub_ui(z, z, (uint64_t) c[i*16+j], GMP_RNDN);

    }
    fprintf(stderr, "\n");
    
    /* y = 2 * y */
    mpfr_mul_ui(y,y, 2, GMP_RNDN); 
    
  }

  fprintf(stderr, "};\n");

  mpfr_clear(t);
  mpfr_clear(z);
  mpfr_clear(y);


  
}


int main(int argc, char* argv[]){

  if(argc != 4){
    fprintf(stderr, "Usage: %s <sigma> <ell> <precision>\n", argv[0]);
    exit(EXIT_FAILURE);

  } else {
    unsigned long int sigma;
    uint32_t ell;
    uint64_t precision;
    
    sigma = atol(argv[1]);
    ell = atoi(argv[2]);
    precision = atol(argv[3]);

    if( (precision < MPFR_PREC_MIN) || (precision >  MPFR_PREC_MAX) ){
      fprintf(stderr, "precision must be between %d and %ld\n", MPFR_PREC_MIN, MPFR_PREC_MAX);
      exit(EXIT_FAILURE);
    }

    mpfr_t ln2, s, ks, f, tmp;

    /* want: 
       k_sigma = ceiling[ sqrt(2*ln 2) * sigma ]
       f = k_sigma^2 / ln 2 
       c[i] = exp(-2^i/f), i = 0 ... ell
    */

    mpfr_init2(ln2, precision);    /* ln(2)   */
    mpfr_init2(s, precision);      /* sigma   */
    mpfr_init2(ks, precision);     /* k_sigma */
    mpfr_init2(f, precision);      /* f       */
    mpfr_init2(tmp, precision);

    mpfr_set_ui(ln2, 2, GMP_RNDN);
    mpfr_log(ln2, ln2, GMP_RNDN);     /* ln2 = ln(2) */
    mpfr_set_ui(s, sigma, GMP_RNDN);  /* s = sigma */
    fprintf(stderr, "\ns = %"PRIu64"\n",  (uint64_t)mpfr_get_d(s, GMP_RNDN));


    mpfr_mul_ui(tmp, ln2, 2, GMP_RNDN);  /* tmp = (2*ln 2)     */

    mpfr_sqrt(tmp, tmp, GMP_RNDN);       /* tmp = sqrt(2*ln 2) */

    mpfr_mul(tmp, s, tmp, GMP_RNDN);    /* tmp = sqrt(2*ln 2) * sigma     */
    mpfr_ceil(ks, tmp);                 /* ks  = ceiling[ sqrt(2*ln 2) * sigma ]    */


    uint64_t k_sigma = (uint64_t)mpfr_get_d(ks, GMP_RNDN);
    fprintf(stderr, "\nk_sigma = %"PRIu64"\n",  k_sigma);


    mpfr_sqr(f, ks, GMP_RNDN);         /* f = k_sigma^2 */
    mpfr_div(f, f, ln2, GMP_RNDN);     /* f = k_sigma^2 / ln 2*/

    double F =  mpfr_get_d(f, GMP_RNDN);
    fprintf(stderr, "\nf = %0.1f\n",  F);

    //mpfr_out_str (stderr, 10, 0, ln2, GMP_RNDN);



    uint8_t* c = calloc(16 * ell, sizeof(uint8_t));

    if(c !=  NULL){ 
      init_c(sigma, ell, c, f, precision);
    }


    
  
    mpfr_clear(ln2);
    mpfr_clear(s);
    mpfr_clear(ks);
    mpfr_clear(f);
    mpfr_clear(tmp);
    
    
    
    exit(EXIT_SUCCESS);
  }
  
}
