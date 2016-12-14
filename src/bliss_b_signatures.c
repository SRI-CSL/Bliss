#include <assert.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
#include "bliss_b_utils.h"
#include "ntt_blzzd.h"




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
  int32_t i, n, q, d, q2, kappa, b_inf, b_l2;
  const bliss_param_t *p;
  int32_t *a, *z1, *z2, *v = NULL, *indices = NULL;
  const int32_t *w, *r;
  uint32_t *c_indices;

  retval = BLISS_B_NO_ERROR;

  p = &public_key->p;
  a = public_key->a;

  
  n = p->n;
  q = p->q;
  d = p->d;
  kappa = p->kappa;
  b_inf = p->b_inf;
  b_l2 = p->b_l2;

  w = p->w;
  r = p->r;

  
  q2 = 2 * q;

  z1 = signature->z1;         /* length n */
  z2 = signature->z2;         /* length n */
  c_indices = signature->c;   /* length kappa */


  
  /* first check the norms */

  if (vector_max_norm(z1, n) > b_inf){
    return BLISS_B_BAD_DATA;
  }

  if((vector_max_norm(z2, n) << d) > b_inf){
    return BLISS_B_BAD_DATA;
  }

  if (vector_scalar_product(z1, z1, n) +
      (vector_scalar_product(z2, z2, n) << (2 * d)) > b_l2){
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


  /* v = a * z1 */
  for (i = 0; i < n; i++)
    v[i] = z1[i];

  ntt32_xmu(v, n, q, v, w);
  ntt32_fft(v, n, q, w);
  ntt32_xmu(v, n, q, v, a);
  ntt32_fft(v, n, q, w);
  ntt32_xmu(v, n, q, v, r);
  ntt32_flp(v, n, q);
  
  /* Saarinen says: verification magic. iam: not sure what is going on here */
  for (i = 0; i < n; i++) {
    if (v[i] & 1){       /* Saarinen says: old: v[i] = ((q + 1) * v[i]) % q2; */
      v[i] += q;
    }
  }

  /*

    // v = v + C * q
    bliss_c_oracle(my_idx, p->kappa, p->n, sign->cseed, p->theta);
    for (i = 0; i < p->kappa; i++)
        v[my_idx[i]] = (v[my_idx[i]] + p->q) % (2 * p->q);

    // drop bits and add z
    for (i = 0; i < p->n; i++) {
        tmp = (((v[i] + (1 << (p->d - 1))) >> p->d) + sign->z[i]) % p->p;
        if (tmp < 0)
            tmp += p->p;
        v[i] = tmp;
    }

    // run the hash on input
    bliss_cseed(cseed, p->theta, mu, mu_len, v, p->n);
    if (memcmp(cseed, sign->cseed, p->theta) != 0) {
        free(v);
        return -5;
    }

  */

  
  return retval;

 fail:

  free(v);
  v = NULL;

  free(indices);
  indices = NULL;

  return retval;

}


/*
// oracle step 1; create a hash from a message and the w[] vector

int bliss_cseed(void *cseed, size_t theta,
    const void *mu, size_t mu_len, const int32_t w[], int n)
{
    int i;
    uint8_t t[2];
    sha3_ctx_t sha;

    sha3_init(&sha, theta);             // SHA3 XOF

    t[0] = n >> 8;                      // encode length n
    t[1] = n & 0xFF;
    sha3_update(&sha, t, 2);

    for (i = 0; i < n; i++) {
        t[0] = w[i] >> 8;               // big endian 16-bit
        t[1] = w[i] & 0xFF;
        sha3_update(&sha, t, 2);
    }
    sha3_update(&sha, mu, mu_len);      // message

    sha3_final(cseed, &sha);

    return 0;
}

// random oracle; takes in a  hash of a message and deterministically
// creates kappa indeces

int bliss_c_oracle(int32_t c_idx[], int kappa, int n,
    const void *cseed, size_t theta)
{
    int i, j, idx;
    sha3_ctx_t sha;
    uint8_t buf[2];

    shake256_init(&sha);
    sha3_update(&sha, cseed, theta);
    shake_xof(&sha);

    for (i = 0; i < kappa;) {
        shake_out(&sha, buf, 2);
        idx = ((((uint16_t) buf[0]) << 8) + ((uint16_t) buf[1])) % n;
        for (j = 0; j < i; j++) {
            if (c_idx[j] == idx)
                break;
        }
        if (j == i)
            c_idx[i++] = idx;
    }

    return 0;
}
*/





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
