#include <inttypes.h>
#include <string.h>

#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
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

static bliss_signature_t signature;

int main(int argc, char* argv[]){

  int32_t retcode;

  char *text = "The lunatics have taken over the asylum";

  uint8_t* msg = (uint8_t*)text;

  size_t msg_sz = strlen(text) + 1;
  
  entropy_init(&entropy, seed);
  
  // fprintf(stderr, "Still need tables for BLISS_B_0 remember!\n");

  retcode = bliss_b_private_key_gen(&private_key, BLISS_B_1, &entropy);
  if (retcode != BLISS_B_NO_ERROR){
    fprintf(stderr, "bliss_b_private_key_gen failed: retcode = %d\n", retcode);
    goto exit;
  }
   
  retcode = bliss_b_public_key_extract(&public_key, &private_key);
  if (retcode != BLISS_B_NO_ERROR){
    fprintf(stderr, "bliss_b_public_key_extract failed: retcode = %d\n", retcode);
    goto exit;
  }

  retcode = bliss_b_sign(&signature,  &private_key, msg, msg_sz, &entropy);  
  if (retcode != BLISS_B_NO_ERROR){
    fprintf(stderr, "bliss_b_sign failed: retcode = %d\n", retcode);
    goto exit;
  }
  
  retcode = bliss_b_verify(&signature,  &public_key, msg, msg_sz);
  if (retcode != BLISS_B_NO_ERROR){
    fprintf(stderr, "bliss_b_verify failed: retcode = %d\n", retcode);
    goto exit;
  }

 exit:
  
  bliss_b_private_key_delete(&private_key);

  bliss_b_public_key_delete(&public_key);

  bliss_signature_delete(&signature);
  
  return 0;
}
