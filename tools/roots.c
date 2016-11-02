#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <gmp.h>




int main(int argc, char* argv[]){
  uint32_t qi, ni, ii;
  mpz_t q, n, r, i;
  
  if(argc != 3){
    fprintf(stderr, "Usage: %s <q (e.g. 12289)> <n (e.g. 512 1024)>\n", argv[0]);
    return 1;
  }

  qi = atol(argv[1]);

  ni = atol(argv[2]);

  mpz_init(q);
  mpz_init(n);
  mpz_init(i);
  mpz_init(r);

  mpz_set_ui(q, qi);
  mpz_set_ui(n, ni);

  
  for(ii = 1; ii < qi; ii++){
    mpz_set_ui(i, ii);
    mpz_powm_sec(r, i, n, q);

    if(!mpz_cmp_ui(r, 1)){
      fprintf(stderr, "%d is a root of unity\n", ii);
    }

  }
  


  mpz_clear(q);
  mpz_clear(n);
  mpz_clear(i);
  mpz_clear(r);




  return 0;
}
