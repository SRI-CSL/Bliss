#include <inttypes.h>

#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "entropy.h"

#include "cpucycles.h"
#include "tests.h"

// hard-coded seed for testing
static uint8_t seed[SHA3_512_DIGEST_LENGTH] = {
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
};


static entropy_t entropy;

static bliss_private_key_t private_key;

static bliss_public_key_t public_key;


int main(int argc, char* argv[]){
  int32_t i, j;

  
  entropy_init(&entropy, seed);

  for (j = BLISS_B_0; j <= BLISS_B_4; j++){

    for (i = 0; i < NTESTS; i++) {
      
      t[i] = cpucycles();
      
      if (bliss_b_private_key_gen(&private_key, j, &entropy) != BLISS_B_NO_ERROR){
	fprintf(stderr, "bliss_b_private_key_gen failed (after %"PRIu32" iterations)\n", i);
	return EXIT_FAILURE;
      }
      
      
      if (bliss_b_public_key_extract(&public_key, &private_key) != BLISS_B_NO_ERROR){
	fprintf(stderr, "bliss_b_public_key_extract failed (after %"PRIu32" iterations)\n", i);
	return EXIT_FAILURE;
      }

      bliss_b_private_key_delete(&private_key);

      bliss_b_public_key_delete(&public_key);



      
    }
    
    fprintf(stdout, "BLISS_B_%d\n", j);
    print_results("bliss_b_private_key_gen bliss_b_public_key_extract: ", t, NTESTS);
    
  }
  
  return 0;
}
