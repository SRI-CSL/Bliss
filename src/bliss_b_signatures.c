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
static int32_t modQ(int32_t x, int32_t q, int32_t q_inv){
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
static void mul2d(int32_t *output, const int32_t *input, int32_t n, int32_t d){
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
static void drop_bits(int32_t *output, const int32_t *input, int32_t n, int32_t d) {
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


int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_private_key_t *private_key, const uint8_t *msg, size_t msg_sz, entropy_t *entropy){
  sampler_t sampler;
  bliss_b_error_t retval;
  const bliss_param_t *p;
  // parameters extracted from p: n = size, q = modulus
  int32_t n, q, kappa;
  // tables of constants for the NTT computations
  const int32_t *w, *r;
  // these are the private key (a is stored as NTT)
  int32_t *a, *s1, *s2;
  // the signature is stored in z1, z2, indices
  int32_t *z1 = NULL, *z2 = NULL,  *indices = NULL;
  // all these are auxiliary buffers, malloc'ed in this function
  int32_t *y1 = NULL, *y2 = NULL, *v = NULL, *v1 = NULL, *v2 = NULL;
  uint8_t *hash = NULL;
  uint32_t i, norm_v, prod_zv;
  size_t hash_sz;
  bool b;

  p = &private_key->p;

  a = private_key->a;
  s1 = private_key->s1;
  s2 = private_key->s2;

  n = p->n;
  q = p->q;

  kappa = p->kappa;

  w = p->w;
  r = p->r;

  /* initialize our sampler */
  if (!sampler_init(&sampler, p->sigma, p->ell, p->precision, entropy)) {
    retval = BLISS_B_BAD_ARGS;
    goto fail;
  }

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

  v1 = malloc(n * sizeof(int32_t));
  if(v1 ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  v2 = malloc(n * sizeof(int32_t));
  if(v2 ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  y1 = malloc(n * sizeof(int32_t));
  if(y1 ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  y2 = malloc(n * sizeof(int32_t));
  if(y2 ==  NULL){
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


  /* 0: compute the hash of the msg */

  /* hash the message into the first SHA3_512_DIGEST_LENGTH bytes of the hash */

  sha3_512(hash, msg, msg_sz);

  /* 1 restart: choose y1, y2 */

 restart:

  for(i = 0; i < n; i++){
    y1[i] = sampler_gauss2(&sampler);
    y2[i] = sampler_gauss2(&sampler);
  }

  /* 2: compute v = ((\xi * (a * y1)) + y2) mod 2q */

  ntt32_xmu(v, n, q, y1, w);  /* multiply by powers of psi */
  ntt32_fft(v, n, q, w);      /* v = ntt(y1) */
  ntt32_xmu(v, n, q, v, a);   /* v = ntt(y1) * a (both in ntt form) */
  ntt32_fft(v, n, q, w);      /* v = ntt(ntt(y1) * a) = inverse ntt(y1) * a modulo reordering */
  ntt32_xmu(v, n, q, v, r);   /* multiply by powers of psi^-1  */
  ntt32_flp(v, n, q);         /* reorder: v * a mod q */

  for (i=0; i<n; i++) {
    // this is v[i] = (2 * y1[i] * one_q2 + y2[i]) % q2
    v[i] = modQ(2*y1[i]*p->one_q2+y2[i], p->q2, p->q2_inv);
  }

  /* 3: generateC of v and the hash of the msg */

  generateC(indices, kappa, v, n, hash, hash_sz);

  /* 4: (v1, v2) = greedySC(c) */

  greedy_sc_blzzd(s1, s2, n,  indices, kappa, v1, v2);

  /* 5: choose a random bit b */

  b = entropy_random_bit(entropy);

  /* 6: (z1, z2) = (z1, z2) + (-1)^b * (v1, v2) */

  if(b){
    for(i = 0; i < n; i++){
      z1[i] = y1[i] + v1[i];
      z2[i] = y2[i] + v2[i];
    }
  } else {
    for(i = 0; i < n; i++){
      z1[i] = y1[i] - v1[i];
      z2[i] = y2[i] - v2[i];
    }
  }

  /* 7: continue with probability 1/(M exp(-|v|^2/2sigma^2) * cosh(<z, v>/sigma^2)) otherwise restart */
  // NOTE: we could do the ber_exp earlier since it does not depend on z
  norm_v = vector_norm2(v1, n) + vector_norm2(v2, n);
  if (! sampler_ber_exp(&sampler, norm_v)) {
    goto restart;
  }
  prod_zv = vector_scalar_product(z1, v1, n) + vector_scalar_product(z1, v1, n);
  if (! sampler_ber_cosh(&sampler, prod_zv)) {
    goto restart;
  }

  /* 8: z2 = (drop_bits(v) - drop_bits(v - z2)) mod p  */
  for (i=0; i<n; i++) {
    y1[i] = v[i] - z2[i];
  }
  drop_bits(v, v, n, p->d);   // drop_bits(v)
  drop_bits(y1, y1, n, p->d); // drop_bits(v - z2)
  for (i=0; i<n; i++) {
    z2[i] = modQ(v[i] - y1[i], q, p->q_inv);
  }
  
  /* 9: seem to also need to check norms akin to what happens in the entry to verify */
  mul2d(y2, z2, n, p->d);
  if (vector_max_norm(z1, n) > p->b_inf) {
    goto restart;
  }
  if (vector_max_norm(y2, n) > p->b_inf) {
    goto restart;
  }
  if (vector_norm2(z1,  n) + vector_norm2(y2, n)  > p->b_l2){
    goto restart;
  }



  /* return (z1, z2, c) */

  signature->p = *p;
  signature->z1 = z1;
  signature->z2 = z2;
  signature->c = (uint32_t *)indices;  //icky cast

  /* need to free some stuff */

  retval = BLISS_B_NO_ERROR;

  goto cleanup;

 fail:

  //zero these puppies out
  if(z1 != NULL){
    zero_memory(z1, n);
    free(z1);
    z1 = NULL;
  }

  //zero these puppies out
  if(z2 != NULL){
    zero_memory(z2, n);
    free(z2);
    z2 = NULL;
  }

  //zero these puppies out
  if(indices != NULL){
    zero_memory(indices, kappa);
    free(indices);
    indices = NULL;
  }

 cleanup:

  free(hash);
  hash = NULL;

  
  //zero these puppies out
  if(v != NULL){
    zero_memory(v, n);
    free(v);
    v = NULL;
  }
	
  //zero these puppies out
  if(y1 != NULL){
    zero_memory(y1, n);
    free(y1);
    y1 = NULL;
  }
	
  //zero these puppies out
  if(y2 != NULL){
    zero_memory(y2, n);
    free(y2);
    y2 = NULL;
  }
	
  //zero these puppies out
  if(v1 != NULL){
    zero_memory(v1, n);
    free(v1);
    v1 = NULL;
  }
	
  //zero these puppies out
  if(v2 != NULL){
    zero_memory(v2, n);
    free(v2);
    v2 = NULL;
  }

  return retval;	
}




int32_t bliss_b_verify(const bliss_signature_t *signature,  const bliss_public_key_t *public_key, const uint8_t *msg, size_t msg_sz){
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

  mul2d(tz2, z2, n, d);

  /* first check the norms */

  if (vector_max_norm(z1, n) > b_inf){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }

  //iam: still not sure why tz2 and not just z2
  if(vector_max_norm(tz2, n) > b_inf){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }

  //iam: still not sure why tz2 and not just z2
  if (vector_norm2(z1, n) + vector_norm2(tz2, n)  > b_l2){
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

  drop_bits(v, v, n, d);

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
