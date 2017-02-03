#include <assert.h>
#include <string.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
#include "bliss_b_utils.h"
#include "ntt_blzzd.h"
#include "shake128.h"


/* iam: comes from FFT.h of bliss-06-13-2013 */
int32_t modQ(int32_t x, int32_t q, int32_t q_inv){
	int64_t y = x;
	if (y < 0){ y += q; }
	y = y - q * ((q_inv * y) >> 32);
	while (y >= q){
	  y -= q;
	}
	x = y;
	return x;
}



/* iam: bliss-06-13-2013 */
void drop_bits(int32_t *output, int32_t *input, int32_t n, int32_t d){
  int32_t i;
  for (i = 0; i < n; i++){
	output[i] = input[i] * (1<<d);
  }
}

/* iam: bliss-06-13-2013
 *
 *   on page 21 of DDLL: every x between [-q, q) and any positive integer d, x can be uniquely written
 *   as  x = [x]_d * 2^d  + r where r is in [-2^(d -1), 2^(d -1)).
 *
 *   we think this is computing: x --> [x]_d
 *
 * we could check this by brute force for q = 12289 and d among 8, 9, 10. Note that this is quite
 * different from the strongswan version.
 *
 */
void drop_bit_shift(int32_t *output, int32_t *input, int32_t n, int32_t d){
  int32_t i;
  for (i = 0; i < n; i++){
	output[i] = (2*input[i] + (1<<d))/(1<<(d + 1));
  }
}


bool generateC(int32_t *indices, size_t kappa, int32_t *n_vector, size_t n, uint8_t *hash, size_t hash_sz){
  int32_t i, j, index;
  uint8_t whash[SHA3_512_DIGEST_LENGTH];
  uint8_t *array;
  uint8_t repetitions;

  array = malloc(n);
  if(array == NULL){
	return false;
  }


  //iam: note that the vector could be int16_t * if we really wanted
  //iam: copy the vector into the front 2 n bytes of hash.
  for(i = 0; i < n; i++){

    //unroll this fucker
	for(j = 0; j < 2; j++){
	  hash[SHA3_512_DIGEST_LENGTH + (2 * i) + j] = n_vector[i]&((uint8_t)-1);
	  n_vector[i] >>= 8;
	}

  }

  repetitions = 0;

 random_oracle:
  repetitions++;
  hash[hash_sz - 1] = repetitions;
  sha3_512(whash, hash, hash_sz);

  memset(array, 0, n);

  j = 0;

  //N = 256

  for(i = 0; i < kappa; ){
	index = whash[j];  // index < 256 < N
	if(!array[index]){
	  indices[i] = index;
	  array[index]++;
	  i++;
	}

	j++;
	if(j >= 64){ goto random_oracle; }

  }

  // N = 512

  free(array);
  return true;

}


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
  int32_t i, n, q, d, mod_p, q2, q_inv, q2_inv, one_q2, kappa, b_inf, b_l2;
  const bliss_param_t *p;
  int32_t *a, *z1, *z2, *tz2, *v = NULL, *indices = NULL;
  const int32_t *w, *r;
  uint32_t *c_indices;

  /* iam: following bliss-06-13-2013 since I get lost when following blzzd  */
  uint8_t *hash = NULL;
  size_t hash_sz;

  p = &public_key->p;
  a = public_key->a;


  n = p->n;
  q = p->q;
  d = p->d;
  mod_p = p-> mod_p;

  kappa = p->kappa;
  b_inf = p->b_inf;
  b_l2 = p->b_l2;

  w = p->w;
  r = p->r;

  q2 = p->q2;
  q_inv =  p->q_inv;
  q2_inv =  p->q2_inv;
  one_q2 = p->one_q2;

  z1 = signature->z1;         /* length n */
  z2 = signature->z2;         /* length n */
  c_indices = signature->c;   /* length kappa */


  /* do the dropped bit shift in tz2 (t for temp);
      iam asks later: why are we doing this again? */

  tz2 = calloc(n, sizeof(int32_t));
  if(tz2 ==  NULL){
     return BLISS_B_NO_MEM;
  }

  drop_bits(tz2, z2, n, d);

  /* first check the norms */

  if (vector_max_norm(z1, n) > b_inf){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }

  if(vector_max_norm(tz2, n) > b_inf){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }

  if (vector_scalar_product(z1, z1, n) + vector_scalar_product(tz2, tz2, n)  > b_l2){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }



  /* make working space */

  hash_sz =  SHA3_512_DIGEST_LENGTH + 2 * n;

  hash = malloc(hash_sz);
  if(hash ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

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

  /* start the real work */

  /* hash the message into the first SHA3_512_DIGEST_LENGTH bytes of the hash */
  sha3_512(hash, msg, msg_sz);

  /* v = a * z1 */
  for (i = 0; i < n; i++)
    v[i] = z1[i];

  ntt32_xmu(v, n, q, v, w);
  ntt32_fft(v, n, q, w);      /* v = ntt(v) */
  ntt32_xmu(v, n, q, v, a);   /* v * a (both in ntt form) */
  ntt32_fft(v, n, q, w);
  ntt32_xmu(v, n, q, v, r);   /* v * a in reversed order */
  ntt32_flp(v, n, q);         /* v * a mod q */

  /* IAM2BD: v = a * z here? now? */

  /*  bliss-06-13-2013 computes  a/(q+2)*z_1+q/(q+2)*c+z_2 =  [(1/(q + 2)) * a * z1] + [(q/q+2) * c] + z_2  mod p */
  /*  iam: there may be some factors missing here; but at least I have something to work with and discuss. */


  /* (1/(q + 2)) * a * z1 */
  for (i = 0; i < n; i++){
	assert(0 <= v[i] && v[i] < q);
	v[i] = modQ(2*v[i]*one_q2, q2, q2_inv);  //iam: why is there a 2 here?
  // TL: because in BLISS and BLISS-B the public key a_1 = is 2*a_q where a_q was "mod q"
  // TL: q2 = 2*q right?
  }

  /*  + (q/q+2) * c */
  for (i = 0; i < kappa; i++) {
	v[c_indices[i]] = modQ(v[c_indices[i]] + (q * one_q2), q2, q2_inv);
  }

  /* iam: huh? */
  /* TL: so we are reconstructing the element to hash, but there is rounding, so we need to do it in the verification again */
  drop_bit_shift(v, v, n, d);

  /*  + z_2  mod p. iam: how many different mod algorithms do we need here? */
  for (i = 0; i < n; i++){
	v[i] += z2[i];
	if (v[i] < 0){
	  v[i] += mod_p;
	}
	if (v[i] >= mod_p){
	  v[i] -= mod_p;
	}
  }

  if(!generateC(indices, kappa, v, n, hash, hash_sz)){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  retval = 1;

  for (i = 0; i < kappa; i++){
	if (indices[i] != c_indices[i]){
	  retval = 0;
	  break;
	}
  }


 fail:

  free(tz2);
  tz2 = NULL;

  free(v);
  v = NULL;

  free(indices);
  indices = NULL;

  free(hash);
  hash = NULL;

  return retval;

}





void bliss_signature_delete(bliss_signature_t *signature){
  //FIXME: do we need to zero here? iam guesses no.
  //TL: No, the signature is not a secret

  assert(signature != NULL);

  free(signature->z1);
  signature->z1 = NULL;

  free(signature->z2);
  signature->z2 = NULL;

  free(signature->c);
  signature->c = NULL;
}
