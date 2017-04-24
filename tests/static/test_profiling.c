#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
#include "entropy.h"

// hard-coded seed for testing
static uint8_t seed[SHA3_512_DIGEST_LENGTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7, 
    0, 1, 2, 3, 4, 5, 6, 7
};

#define NTESTS 1024 * 4

static entropy_t entropy;

static bliss_private_key_t private_key;

static bliss_public_key_t public_key;

static bliss_signature_t signature;

int main(int argc, char* argv[]) {
  int32_t type;
  int32_t count = 0;
  int32_t retcode;

  char* text = "The lunatics have taken over the asylum";
  uint8_t* msg = (uint8_t*)text;
  size_t msg_sz = strlen(text) + 1;

  entropy_init(&entropy, seed);

  for (type = BLISS_B_0; type <= BLISS_B_4; type++) {
    retcode = bliss_b_private_key_gen(&private_key, type, &entropy);
    if (retcode != BLISS_B_NO_ERROR) {
      fprintf(stderr,
              "bliss_b_private_key_gen failed: type = %d, retcode = %d\n", type,
              retcode);
      goto exit;
    }

    retcode = bliss_b_public_key_extract(&public_key, &private_key);
    if (retcode != BLISS_B_NO_ERROR) {
      fprintf(stderr,
              "bliss_b_public_key_extract failed: type = %d, retcode = %d\n",
              type, retcode);
      goto exit;
    }

    for (count = 0; count < NTESTS; count++) {
      retcode = bliss_b_sign(&signature, &private_key, msg, msg_sz, &entropy);
      if (retcode != BLISS_B_NO_ERROR) {
        fprintf(stderr, "bliss_b_sign failed: type = %d, retcode = %d\n", type,
                retcode);
        goto exit;
      }

      retcode = bliss_b_verify(&signature, &public_key, msg, msg_sz);
      if (retcode != BLISS_B_NO_ERROR) {
        fprintf(stderr, "bliss_b_verify failed: type = %d, retcode = %d\n",
                type, retcode);
        goto exit;
      }

      bliss_signature_delete(&signature);

      if (count % 100 == 0) {
        fprintf(stdout, ".");
        fflush(stdout);
      }
    }

  exit:

    bliss_b_private_key_delete(&private_key);

    bliss_b_public_key_delete(&public_key);

    fprintf(stdout, "\n");
  }
  return 0;
}
