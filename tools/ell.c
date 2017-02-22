#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <mpfr.h>



int main(int argc, char* argv[]){

  if(argc != 2){
    fprintf(stderr, "Usage: %s <sigma>\n", argv[0]);
    exit(EXIT_FAILURE);

  } else {
    uint32_t sigma;
    uint32_t precision = 128;
    
    sigma = atoi(argv[1]);

    mpfr_t sq2r2;
    mpfr_t inv_sq2r2;
    mpfr_t k, B;

    int32_t ell = 0;

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
    mpfr_init2(B, precision);   

    mpfr_set_ui(sq2r2, 2, GMP_RNDN);    /* sq2r2 = 2 */
    mpfr_log(sq2r2, sq2r2, GMP_RNDN);     /* sq2r2 = ln(2) */
    mpfr_mul_ui(sq2r2, sq2r2, 2, GMP_RNDN);     /* sq2r2 = 2*ln(2)  */
    mpfr_sqrt(sq2r2, sq2r2, GMP_RNDN);       /* sq2r2 = sqrt(2*ln 2) */


    mpfr_set_ui(inv_sq2r2, 1, GMP_RNDN);    /* inv_sq2r2 = 1 */
    mpfr_div(inv_sq2r2, inv_sq2r2, sq2r2, GMP_RNDN);     /* inv_sq2r2 = 1  /  sqrt(2*ln 2*/

    mpfr_mul_ui(k, sq2r2, sigma, GMP_RNDN);     /* k = sqrt(2 ln 2) * sigma  */
    mpfr_add_ui(k, k, 1, GMP_RNDN);             /* k = sqrt(2 ln 2) * sigma + 1  */

    
    double sq2ln2 = mpfr_get_d(sq2r2, GMP_RNDN);

    double inv_sq2ln2 = mpfr_get_d(inv_sq2r2, GMP_RNDN);

    fprintf(stdout, "\nstatic const double sq2ln2 = %f;\n", sq2ln2);

    fprintf(stdout, "\nstatic const double inv_sq2ln2 = %f;\n", inv_sq2ln2);

    
  
    mpfr_clear(sq2r2);
    mpfr_clear(inv_sq2r2);
    mpfr_clear(k);
    mpfr_clear(B);
    
    exit(EXIT_SUCCESS);
  }
  
}
