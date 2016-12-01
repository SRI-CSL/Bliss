#include <inttypes.h>

#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "entropy.h"

#include "cpucycles.h"
#include "tests.h"



static entropy_t entropy;


static bliss_private_key_t private_key;


int main(int argc, char* argv[]){
  int32_t i;

  
  entropy_init(&entropy, seed);


  for (i = 0; i < NTESTS; i++) {

    fprintf(stderr, "i = %"PRIu32"\n",  i);
    
    t[i] = cpucycles();

    if (bliss_b_private_key_gen(&private_key, BLISS_B_0, &entropy) != BLISS_B_NO_ERROR){
     fprintf(stderr, "bliss_b_private_key_gen failed (after %"PRIu32" iterations)\n", i);
     return EXIT_FAILURE;
    }

    bliss_b_private_key_delete(&private_key);
    
  }

  print_results("bliss_b_private_key_gen  BLISS_B_0: ", t, NTESTS);


  return 0;
}
