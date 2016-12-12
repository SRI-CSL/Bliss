#include <assert.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
#include "bliss_b_utils.h"




int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_private_key_t *private_key, const uint8_t *msg, size_t msg_sz, entropy_t *entropy){
  int32_t n, q;
  const bliss_param_t *p;

  p = &private_key->p;

  n = p->n;
  q = p->q;


  return BLISS_B_NO_ERROR;
}





int32_t bliss_b_verify(bliss_signature_t *signature,  const bliss_public_key_t *public_key, const uint8_t *msg, size_t msg_sz){
  bliss_b_error_t retval;
  int32_t n, q, q2, kappa, b_inf, b_l2;
  const bliss_param_t *p;
  int32_t *z1, *z2, *v = NULL, *indices = NULL;
  uint32_t *c_indices;

  retval = BLISS_B_NO_ERROR;

  p = &public_key->p;

  n = p->n;
  q = p->q;
  kappa = p->kappa;
  b_inf = p->b_inf;
  b_l2 = p->b_l2;

  q2 = 2 * q;

  z1 = signature->z1;         /* length n */
  z2 = signature->z2;         /* length n */
  c_indices = signature->c;   /* length kappa */

  /* first check the norms */

  if (vector_max_norm(z1, n) > b_inf){
    return BLISS_B_BAD_DATA;
  }

  if((vector_max_norm(z2, n) << p->d) > b_inf){
    return BLISS_B_BAD_DATA;
  }

  if (vector_scalar_product(z1, z1, n) +
      (vector_scalar_product(z2, z2, n) << (2 * p->d)) > b_l2){
    return BLISS_B_BAD_DATA;
  }


  /* make working space */

  v = calloc(n, sizeof(int32_t));
  if(v ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  indices = calloc(kappa, sizeof(int32_t));
  if(indices ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }



  return retval;

 fail:

  free(v);
  v = NULL;

  free(indices);
  indices = NULL;

  return retval;

}





void bliss_signature_delete(bliss_signature_t *signature){
  //FIXME: do we need to zero here? iam guesses no.

  assert(signature != NULL);

  free(signature->z1);
  signature->z1 = NULL;

  free(signature->z2);
  signature->z2 = NULL;

  free(signature->c);
  signature->c = NULL;
}
