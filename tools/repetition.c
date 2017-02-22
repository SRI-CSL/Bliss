#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <mpfr.h>



int main(int argc, char* argv[]){

  if(argc != 3){
    fprintf(stderr, "Usage: %s <sigma> <repetition rate: m> \n", argv[0]);
    exit(EXIT_FAILURE);

  } else {
    uint32_t sigma;
    double rr;
    uint32_t precision = 128;
    
    sigma = atoi(argv[1]);
    rr = strtod(argv[2], NULL);

    mpfr_t lnrr, s;

    mpfr_init2(lnrr, precision);    /* ln(rr)   */
    mpfr_init2(s, precision);       /* sigma   */

    mpfr_set_d(lnrr, rr, GMP_RNDN);
    mpfr_log(lnrr, lnrr, GMP_RNDN);     /* lnrr = ln(rr) */
    mpfr_set_ui(s, sigma, GMP_RNDN);    /* s = sigma */
    mpfr_mul(s, s, s, GMP_RNDN);        /* s = sigma^2 */

    mpfr_mul_ui(s, s, 2, GMP_RNDN);     /* s = (2*sigma^2)     */

    mpfr_mul(s, s, lnrr, GMP_RNDN);     /* s = 2*sigma^2 * ln(rr)     */

    mpfr_ceil(s, s);                    /* s  = ceiling[ 2*sigma^2 * ln(rr) ]   */

    double M = mpfr_get_d(s, GMP_RNDN);

    fprintf(stdout, "\nstatic const double M = %f;\n", M);
    
  
    mpfr_clear(lnrr);
    mpfr_clear(s);
    
    exit(EXIT_SUCCESS);
  }
  
}
