#ifndef __BLISS_B_SIGNATURES_H__
#define __BLISS_B_SIGNATURES_H__

#include <stdint.h>
#include "bliss_b_params.h"
#include "entropy.h"


typedef struct {
  bliss_param_t p;                   /* parameter set          */
  int32_t *t;                        /* signature t            */
  int32_t *z;                        /* signature z            */
  uint8_t *cseed;                    /* seed for c_idx         */
} bliss_signature_t;


/*  Generates a signature of a message given a bliss_b private key.
 *  
 *  - signature; structure to store the result
 *  - private_key; a valid bliss-b private key
 *  - msg; the message to be signed
 *  - msg_sz; the size of the message
 *
 *  Returns 0 on success, or a negative error code on failure.
 */

extern int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_b_private_key_t private_key, const void *msg, size_t msg_sz);


extern int32_t bliss_b_verify(bliss_signature_t *signature,  const bliss_b_public_key_t public_key, const void *msg, size_t msg_sz);


extern void bliss_signature_delete(bliss_signature_t *signature);


#endif

