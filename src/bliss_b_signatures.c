#include <assert.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"


int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_private_key_t private_key, const void *msg, size_t msg_sz){


  return BLISS_B_NO_ERROR;
}


int32_t bliss_b_verify(bliss_signature_t *signature,  const bliss_public_key_t public_key, const void *msg, size_t msg_sz){


  return BLISS_B_NO_ERROR;
}


void bliss_signature_delete(bliss_signature_t *signature){
  assert(signature != NULL);

  free(signature->z1);
  signature->z1 = NULL;

  free(signature->z2);
  signature->z2 = NULL;

  free(signature->c);
  signature->c = NULL;
}

