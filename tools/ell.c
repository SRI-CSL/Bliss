#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <mpfr.h>


static uint32_t  get_bits(uint64_t num){
  uint32_t retval = 0;

  while(num > 0){
    num >>= 1;
    retval++;
  }
  return retval;
}



int main(int argc, char* argv[]){

  if(argc != 3){
    fprintf(stderr, "Usage: %s <sigma> <precision>\n", argv[0]);
    exit(EXIT_FAILURE);

  } else {
    uint32_t sigma;
    uint32_t precision;
    
    sigma = atoi(argv[1]);
    precision = atoi(argv[2]);

    mpfr_t sq2r2;
    mpfr_t inv_sq2r2;
    mpfr_t k, k_minus_one, B;

    /*
        tau = 12

        sigma_bin_inv_lowprec = sqrt(2 ln 2)
 
	sigma_bin_lowprec = sqrt(1/(2 ln 2))

	k = (unsigned long) (sigma_bin_inv_lowprec*sigma) + 1
          = sqrt(2 ln 2) * sigma + 1

	unsigned long B = (unsigned long) (k-1)*(k-1+2*k*sigma_bin_lowprec*tau)+1

        sigma_bin_lowprec = sqrt(1/(2 ln 2))
	
	ell=0;
	while (B>0) {
		B=B>>1;
		ell++;
	}

    */

    
    mpfr_init2(sq2r2, precision);       /* sqrt(2 ln 2) */
    mpfr_init2(inv_sq2r2, precision);   /* 1/sqrt(2 ln 2) */
    mpfr_init2(k, precision);   
    mpfr_init2(k_minus_one, precision);   
    mpfr_init2(B, precision);   

    mpfr_set_ui(sq2r2, 2, GMP_RNDN);         /* sq2r2 = 2 */
    mpfr_log(sq2r2, sq2r2, GMP_RNDN);        /* sq2r2 = ln(2) */
    mpfr_mul_ui(sq2r2, sq2r2, 2, GMP_RNDN);  /* sq2r2 = 2*ln(2)  */
    mpfr_sqrt(sq2r2, sq2r2, GMP_RNDN);       /* sq2r2 = sqrt(2*ln 2) */

    mpfr_set_ui(inv_sq2r2, 1, GMP_RNDN);                /* inv_sq2r2 = 1 */
    mpfr_div(inv_sq2r2, inv_sq2r2, sq2r2, GMP_RNDN);    /* inv_sq2r2 = 1/sqrt(2*ln 2) */

    mpfr_mul_ui(k_minus_one, sq2r2, sigma, GMP_RNDN);   /* k-1 = sqrt(2 ln 2) * sigma  */
    mpfr_add_ui(k, k_minus_one, 1, GMP_RNDN);           /* k = sqrt(2 ln 2) * sigma + 1  */

    mpfr_set_ui(B, 2 * 12, GMP_RNDN);           /* B = 2 * tau */
    mpfr_mul(B, B, inv_sq2r2, GMP_RNDN);
    mpfr_mul(B, B, k, GMP_RNDN);                /* B = 2 * k * (1/sqrt(2 ln 2)) * tau */
    mpfr_add(B, B, k_minus_one, GMP_RNDN);      /* B = (k - 1) + 2 * k * (1/sqrt(2 ln 2)) * tau */
    mpfr_mul(B, B, k_minus_one, GMP_RNDN);
    mpfr_add_ui(B, B, 1, GMP_RNDN);

    unsigned long bb = mpfr_get_ui(B, GMP_RNDN);
    
    double sq2ln2 = mpfr_get_d(sq2r2, GMP_RNDN);

    double inv_sq2ln2 = mpfr_get_d(inv_sq2r2, GMP_RNDN);

    fprintf(stdout, "\nstatic const double sq2ln2 = %f;\n", sq2ln2);

    fprintf(stdout, "\nstatic const double inv_sq2ln2 = %f;\n", inv_sq2ln2);

    fprintf(stdout, "\nstatic const uint32_t bb = %lu;\n", bb);

    fprintf(stdout, "\nstatic const uint32_t ell = %u;\n", get_bits(bb));
  
    mpfr_clear(sq2r2);
    mpfr_clear(inv_sq2r2);
    mpfr_clear(k);
    mpfr_clear(k_minus_one);
    mpfr_clear(B);
    
    exit(EXIT_SUCCESS);
  }
  
}
