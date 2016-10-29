#include <stdio.h>
#include <stdlib.h>
#include <mpfr.h>




int main(int argc, char* argv[]){

  if(argc != 4){
    fprintf(stderr, "Usage: %s <sigma> <ell> <precision>\n", argv[0]);
    exit(EXIT_FAILURE);

  } else {
    uint32_t sigma, l;
    uint64_t precision;
    
    sigma = atoi(argv[1]);
    l = atoi(argv[2]);
    precision = atol(argv[3]);

    if( (precision < MPFR_PREC_MIN) || (precision >  MPFR_PREC_MAX) ){
      fprintf(stderr, "precision must be between %d and %ld\n", MPFR_PREC_MIN, MPFR_PREC_MAX);
      exit(EXIT_FAILURE);
    }

    mpfr_t ln2;

    mpfr_init2(ln2, precision);

    mpfr_set_ui(ln2, 2, GMP_RNDN);
    mpfr_log(ln2, ln2, GMP_RNDN);    /* ln2 = ln(2) */

    double ln2_d =  mpfr_get_d(ln2, GMP_RNDN);

    mpfr_out_str (stderr, 10, 0, ln2, GMP_RNDN);
    fprintf(stderr, "\nln(2) = %0.32f\n",  ln2_d);
  
    mpfr_clear(ln2);
    
    
    exit(EXIT_SUCCESS);
  }
  
}
