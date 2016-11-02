#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>


static bool is_nth_root_of_unity_mod_q(uint32_t ii, uint32_t ni, uint32_t qi){
  bool retval;
  mpz_t q, n, r, i;
  
  mpz_init(q);
  mpz_init(n);
  mpz_init(i);
  mpz_init(r);

  mpz_set_ui(q, qi);
  mpz_set_ui(n, ni);

  mpz_set_ui(i, ii);
  mpz_powm_sec(r, i, n, q);

  retval = mpz_cmp_ui(r, 1) == 0 ? true : false;
  
  mpz_clear(q);
  mpz_clear(n);
  mpz_clear(i);
  mpz_clear(r);

  return retval;
}


static bool is_primitive_nth_root_of_unity_mod_q(uint32_t ii, uint32_t ni, uint32_t qi){
  uint32_t ri;
  
  if(is_nth_root_of_unity_mod_q(ii, ni, qi)){
    for( ri = 1; ri <  ni; ri++){
      if(is_nth_root_of_unity_mod_q(ii, ri, qi)){   return false; }
    }

    return true;

  } 

  return false;

}



int main(int argc, char* argv[]){
  uint32_t qi, ni, ii;

  uint32_t ucount, pcount;
  
  if(argc != 3){
    fprintf(stderr, "Usage: %s <q (e.g. 12289)> <n (e.g. 512 1024)>\n", argv[0]);
    return 1;
  }

  qi = atol(argv[1]);

  ni = atol(argv[2]);

  ucount = 0;

  pcount = 0;
  
  for(ii = 1; ii < qi; ii++){

    if(is_nth_root_of_unity_mod_q(ii, ni, qi)){
      fprintf(stderr, "%d is a root of unity", ii);
      ucount++;
      if(is_primitive_nth_root_of_unity_mod_q(ii, ni, qi)){
	pcount++;
	fprintf(stderr, " and is primitive!\n");
      } else {
	fprintf(stderr, ".\n");
      }
    }
  }

  fprintf(stderr, "%d roots of which %d are primitive\n", ucount, pcount);

  return 0;
}
