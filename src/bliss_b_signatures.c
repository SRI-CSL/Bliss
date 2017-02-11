#include <assert.h>
#include <string.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
#include "bliss_b_utils.h"
#include "ntt_blzzd.h"
#include "sampler.h"
#include "shake128.h"


/* iam: comes from FFT.h of bliss-06-13-2013 */
/* TL: should probably make it constant time */
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

  assert(0 < d && d < 31);

  for (i = 0; i < n; i++){
    output[i] = input[i] << d;
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
void drop_bit_shift(int32_t *output, int32_t *input, int32_t n, int32_t d) {
  int32_t i, delta, half_delta;

  assert(0 < d && d < 31);

  delta = 1<<d;
  half_delta = delta >> 1;
  for (i = 0; i < n; i++) {
    //    output[i] = (2*input[i] + (1<<d))/(1<<(d + 1));
    output[i] = (input[i] + half_delta) / delta;
  }
}


void generateC(int32_t *indices, size_t kappa, int32_t *n_vector, size_t n, uint8_t *hash, size_t hash_sz){
  uint8_t whash[SHA3_512_DIGEST_LENGTH];
  uint8_t array[512]; // size we need is either 256 (for Bliss 0) or 512 for others
  int32_t i, j, index;
  uint32_t x;
  uint8_t extra_bits;

  assert(n <= 512 && hash_sz == SHA3_512_DIGEST_LENGTH + 2 * n);

  //iam: note that the vector could be int16_t * if we really wanted
  /*
   * append the n_vector to the hash array
   */
  j = SHA3_512_DIGEST_LENGTH;
  for(i = 0; i < n; i++) {
    // n_vector[i] is between 0 and mod_p (less than 2^16)
    x = (uint32_t) n_vector[i];
    hash[j] = x & 255;
    hash[j + 1] = (x >> 8);
    j += 2;
  }

  while (true) {
    // BD: just to be safe, we shouldn't overwrite the last element of hash
    // (so that n_vector[n-1] is taken into account).
    hash[hash_sz - 1] ++;
    sha3_512(whash, hash, hash_sz);

    memset(array, 0, n);

    if (n == 256) {
      // Bliss_b 0: we need kappa indices of 8 bits
      i = 0;
      for (j=0; j<SHA3_512_DIGEST_LENGTH; j++) {
	index = whash[j];  // index < 256
	if(! array[index]) {
	  indices[i] = index;
	  array[index] ++;
	  i ++;
	  if (i >= kappa) return;
	}
      }

    } else {
      assert(n == 512 && (SHA3_512_DIGEST_LENGTH & 7) == 0);
      // we need kappa indices of 9 bits
      i = 0;
      j = 0;
      while(j<SHA3_512_DIGEST_LENGTH) {
	if ((j & 7) == 0) {
	  // start of a block of 8 bytes
	  extra_bits = whash[j];
	  j ++;
	}
	index = (whash[j] << 1) | (extra_bits & 1);
	extra_bits >>= 1;
	j ++;

	if(! array[index]) {
	  indices[i] = index;
	  array[index] ++;
	  i ++;
	  if (i >= kappa) return;
	}
      }
    }
  }
}

/*
 * hard-coded seed for sampler: need to get this from somewhere
 */
int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_private_key_t *private_key, const uint8_t *msg, size_t msg_sz, entropy_t *entropy){
  sampler_t sampler;
  bliss_b_error_t retval;
  int32_t i, n, q;
  const bliss_param_t *p;
  int32_t *a, *z1 = NULL, *z2 = NULL;
  uint8_t *hash = NULL;
  size_t hash_sz;

  p = &private_key->p;
  a = private_key->a;

  n = p->n;
  q = p->q;

  /* make working space */
  hash_sz =  SHA3_512_DIGEST_LENGTH + 2 * n;

  hash = malloc(hash_sz);
  if(hash ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }


  z1 = malloc(n * sizeof(int32_t));
  if(z1 ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  z2 = malloc(n * sizeof(int32_t));
  if(z2 ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  //iam: need to make a more educated guess as to what these parameters are.
  if (!sampler_init(&sampler, p->sigma, p->ell, p->precision, entropy)) {
    retval = BLISS_B_BAD_ARGS;
    goto fail;
  }

  
  /* 0: compute the hash of the msg */


  /* hash the message into the first SHA3_512_DIGEST_LENGTH bytes of the hash */

  sha3_512(hash, msg, msg_sz);
  
  /* 1 restart: choose y1, y2 */

 restart:

  for(i = 0; i < n; i++){
	z1[i] = sampler_gauss2(&sampler);
	z2[i] = sampler_gauss2(&sampler);
  }

  
  /* 2: compute u = \xi * a1 * y1 + y2 mod 2q */

  /* 3: generateC of u and the hash of the msg */

  /* 4: (v1, v2) = greedySC(c) */

  /* 5: choose a random bit b */

  /* 6: (z1, z2) = (y1, y2) + (-1)^b * (v1, v2) */

  /* 7: continue with sampler_gauss2 probability otherwise restart */

  
  /* 8: z2 = (drop_bits(u) - drop_bits(u - z2)) mod p  */

  /* 9: seem to also need to check norms akin to what happens in the entry to verify */

  /* return (z1, z2, c) */
	 

  return BLISS_B_NO_ERROR;

 fail:

  free(hash);
  hash = NULL;

  //zero these puppies out
  free(z1);
  z1 = NULL;

  //zero these puppies out
  free(z2);
  z2 = NULL;

  return retval;
  
}





int32_t bliss_b_verify(bliss_signature_t *signature,  const bliss_public_key_t *public_key, const uint8_t *msg, size_t msg_sz){
  bliss_b_error_t retval;
  int32_t i, n, q, d, mod_p, q2, q_inv, q2_inv, one_q2, kappa, b_inf, b_l2;
  const bliss_param_t *p;
  int32_t *a, *z1, *z2, *tz2, *v = NULL, *indices = NULL;
  const int32_t *w, *r;
  uint32_t *c_indices;

  uint8_t *hash = NULL;
  size_t hash_sz;

  p = &public_key->p;
  a = public_key->a;


  n = p->n;
  q = p->q;
  d = p->d;
  mod_p = p->mod_p;

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
  // TL: Probably because the infinity and L2 bounds given in BLISS are for "z1" and a "z2" of similar size. This comes from how we assessed security from the attacks.

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
  /* TL: Should do this in constant time */
  for (i = 0; i < n; i++){
	v[i] += z2[i];
	if (v[i] < 0){
	  v[i] += mod_p;
	}
	if (v[i] >= mod_p){
	  v[i] -= mod_p;
	}
  }

  generateC(indices, kappa, v, n, hash, hash_sz);
  retval = BLISS_B_NO_ERROR;

  for (i = 0; i < kappa; i++){
    if (indices[i] != c_indices[i]){
      retval = BLISS_B_VERIFY_FAIL;
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
