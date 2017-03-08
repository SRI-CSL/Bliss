#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"
#include "bliss_b_utils.h"
#include "sampler.h"
#include "shake128.h"
#include "modulii.h"

#include "ntt_api.h"

#define VERBOSE_RESTARTS  false


/* iam: bliss-06-13-2013 */
static void mul2d(int32_t *output, const int32_t *input, uint32_t n, uint32_t d){
  uint32_t i;

  assert(0 < d && d < 31);

  for (i = 0; i < n; i++){
    output[i] = input[i] << d;
  }
}

#ifndef NDEBUG
static bool check_arg(int32_t v[], uint32_t n, int32_t q){
  uint32_t i;

  for(i = 0; i < n; i++){
    if(v[i] < 0){ return false; }
    if(v[i] >= q){ return false; }
  }

  return true;
}
#endif

/* iam: bliss-06-13-2013
 *
 *   on page 21 of DDLL: every x between [-q, q) and any positive integer d, x can be uniquely written
 *   as  x = [x]_d * 2^d  + r where r is in [-2^(d -1), 2^(d -1)).
 *
 *   this is computing: x --> [x]_d
 *
 */
static void drop_bits(int32_t *output, const int32_t *input, uint32_t n, uint32_t d) {
  uint32_t i;
  int32_t  delta, half_delta;

  assert(0 < d && d < 31);

  delta = 1<<d;
  half_delta = delta >> 1;
  for (i = 0; i < n; i++) {
    output[i] = (input[i] + half_delta) / delta;
  }
}

/*
 * GreedySC (derived from blzzd version)
 *
 * should be static once we choose one and use it.
 *
 * Input:  s1, s2, are the polynomial components of the secret key.
 *         c_indices correspond to the sparse polynomial
 *
 * Output: v1 and v2 are output polynomials of size n.
 *
 */
static void greedy_sc(const int32_t *s1, const int32_t *s2, uint32_t n,  const uint32_t *c_indices, uint32_t kappa, int32_t *v1, int32_t *v2){
  uint32_t index, i, k;
  int32_t sign;

  for (i = 0; i < n; i++) {
    v1[i] = 0;
    v2[i] = 0;
  }

  for (k = 0; k < kappa; k++) {

    index = c_indices[k];
    sign = 0;
    /* \xi_i = sign(<v, si>) */
    for (i = 0; i < n - index; i++) {
      sign += s1[i] * v1[index + i] + s2[i] * v2[index + i];
    }
    for (i = n - index; i < n; i++) {
      sign -= s1[i] * v1[index + i - n] + s2[i] * v2[index + i - n];
    }
    /* v = v - \xi_i . si */
    if (sign > 0) {
      for (i = 0; i < n - index; i++) {
	v1[index + i] -= s1[i];
	v2[index + i] -= s2[i];
      }
      for (i = n - index; i < n; i++) {
	v1[index + i - n] += s1[i];
	v2[index + i - n] += s2[i];
      }
    } else {
      for (i = 0; i < n - index; i++) {
	v1[index + i] += s1[i];
	v2[index + i] += s2[i];
      }
      for (i = n - index; i < n; i++) {
	v1[index + i - n] -= s1[i];
	v2[index + i - n] -= s2[i];
      }
    }
  }
}



static void generateC(uint32_t *indices, uint32_t kappa, const int32_t *n_vector, uint32_t n, uint8_t *hash, uint32_t hash_sz){
  uint8_t whash[SHA3_512_DIGEST_LENGTH];
  uint8_t array[512]; // size we need is either 256 (for Bliss 0) or 512 for others
  uint32_t i, j, index;
  uint32_t x, tries;
  uint32_t extra_bits;

  assert(n <= 512 && hash_sz == SHA3_512_DIGEST_LENGTH + 2 * n);

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

  /* We bail out after 256 iterations in case something goes wrong. */
  for (tries = 0; tries < 256; tries ++) {
    /*
     * BD: just to be safe, we shouldn't overwrite the last element of hash
     * (so that n_vector[n-1] is taken into account).
     */
    hash[hash_sz - 1] ++;
    sha3_512(whash, hash, hash_sz);

    memset(array, 0, n);

    if (n == 256) {
      /* Bliss_b 0: we need kappa indices of 8 bits */
      i = 0;
      for (j=0; j<SHA3_512_DIGEST_LENGTH; j++) {
	index = whash[j];  
	if(! array[index]) {
	  indices[i] = index;
	  array[index] = 1;
	  i ++;
	  if (i >= kappa) return;
	}
      }

    } else {
      assert(n == 512 && (SHA3_512_DIGEST_LENGTH & 7) == 0);

      extra_bits = 0; // Prevent a GCC warning

      /* We need kappa indices of 9 bits */
      i = 0;
      j = 0;
      while(j<SHA3_512_DIGEST_LENGTH) {
	if ((j & 7) == 0) {
	  /* start of a block of 8 bytes */
	  extra_bits = whash[j];
	  j ++;
	}
	index = ((uint32_t)whash[j] << 1) | (extra_bits & 1);
	extra_bits >>= 1;
	j ++;

	if(! array[index]) {
	  indices[i] = index;
	  array[index] = 1;
	  i ++;
	  if (i >= kappa) return;
	}
      }
    }
  }
}

/*
 * Auxiliary function: add s * c to z
 * - c = array of kappa indices
 */
static void addmul_c(int32_t *z, uint32_t n, const int32_t *s, const uint32_t *c_indices, uint32_t kappa) {
  uint32_t i, j, idx;

  for (i=0; i<kappa; i++) {
    idx = c_indices[i];
    // add s * X^idx to z
    for (j=0; j<idx; j++) {
      z[j] -= s[n - idx + j];
    }
    for (j=idx; j<n; j++) {
      z[j] += s[j - idx];
    }
  }
}

/*
 * Auxiliary function: subtract s * c from z
 * - c = array of kappa indices
 */
static void submul_c(int32_t *z, uint32_t n, const int32_t *s, const uint32_t *c_indices, uint32_t kappa) {
  uint32_t i, j, idx;

  for (i=0; i<kappa; i++) {
    idx = c_indices[i];
    // subtract s * X^idx to z
    for (j=0; j<idx; j++) {
      z[j] += s[n - idx + j];
    }
    for (j=idx; j<n; j++) {
      z[j] -= s[j - idx];
    }
  }
}


/*
 * BD: Consistency check for v, y1, y2
 * - v is (2 * zeta * y1 * a1 + y2)
 * - for any c, we can build 
 *    z1 = y1 + c * s1
 *    z2 = y2 + c * s2
 * then we should have 
 *  (2 * zeta * a * z1 + zeta * q * c + z2) == v mod 2q
 */
static void check_before_drop(const bliss_private_key_t *key, uint8_t *hash, uint32_t hash_sz,
			      const int32_t *v, const int32_t *y1, const int32_t *y2, bliss_param_t *p, ntt_state_t state) {
  int32_t z1[512], z2[512], aux[512];
  uint32_t c[40];
  int32_t q;
  uint32_t kappa, n, i, idx;
  bool ok;

  n = p->n;
  q = p->q;
  kappa = p->kappa;

  assert(n <= 512 && kappa <= 40);
  generateC(c, kappa, v, n, hash, hash_sz);

  // first check
  for (i=0; i<n; i++) {
    z1[i] = y1[i];
    z2[i] = y2[i];
  }
  addmul_c(z1, n, key->s1, c, kappa);
  addmul_c(z2, n, key->s2, c, kappa);

  // make sure there's no overflow in xmu
  for (i=0; i<n; i++) {
    z1[i] = z1[i] % q;
  }

  // compute z1 * a in aux
  multiply_ntt(state, aux, z1, key->a);
    

  for (i=0; i<n; i++) {
    aux[i] = 2 * aux[i] * p->one_q2 + z2[i];
  }
  for (i=0; i<kappa; i++) {
    idx = c[i];
    aux[idx] += p->one_q2 * q;
  }
  for (i=0; i<n; i++) {
    aux[i] = aux[i] % p->q2;
    if (aux[i] < 0) aux[i] += p->q2;
  }

  ok = true;
  
  for (i = 0; i < n; i++) {
    if (v[i] != aux[i]) {
      ok = false;
      break;
    }
  }
 
  if (ok) {
    printf("\nCONSISTENCY CHECK 1 PASSED\n");
  } else {
    printf("\nCONSISTENCY CHECK 1 FAILED\n");
    printf("v is:\n");
    for (i=0; i<n; i++) {
      printf(" %d", v[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
    printf("aux is:\n");
    for (i=0; i<n; i++) {
      printf(" %d", aux[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
  }


  // second check: subtract
  for (i=0; i<n; i++) {
    z1[i] = y1[i];
    z2[i] = y2[i];
  }
  submul_c(z1, n, key->s1, c, kappa);
  submul_c(z2, n, key->s2, c, kappa);

  // make sure there's no overflow in xmu
  for (i=0; i<n; i++) {
    z1[i] = z1[i] % q;
  }

  // compute z1 * a in aux
  multiply_ntt(state, aux, z1, key->a);
 
  for (i=0; i<n; i++) {
    aux[i] = 2 * aux[i] * p->one_q2 + z2[i];
  }
  for (i=0; i<kappa; i++) {
    idx = c[i];
    aux[idx] += p->one_q2 * q;
  }
  for (i=0; i<n; i++) {
    aux[i] = aux[i] % p->q2;
    if (aux[i] < 0) aux[i] += p->q2;
  }

  ok = true;
  
  for(i = 0; i < n; i++){
    if (v[i] != aux[i]){
      ok = false;
      break;
    }
  }
 
  if (ok) {
    printf("\nCONSISTENCY CHECK 2 PASSED\n\n");
  } else {
    printf("\nCONSISTENCY CHECK 2 FAILED\n");
    printf("v is:\n");
    for (i=0; i<n; i++) {
      printf(" %d", v[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
    printf("aux is:\n");
    for (i=0; i<n; i++) {
      printf(" %d", aux[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n\n");
  }

}

int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_private_key_t *private_key, const uint8_t *msg, size_t msg_sz, entropy_t *entropy){
  sampler_t sampler;
  bliss_b_error_t retval;
  bliss_param_t p;
  ntt_state_t state;

  // parameters extracted from p: n = size, q = modulus
  uint32_t n, kappa;
  // these are the private key (a is stored as NTT)
  int32_t *a, *s1, *s2;
  // the signature is stored in z1, z2, indices
  int32_t *z1 = NULL, *z2 = NULL;
  uint32_t *indices = NULL;
  // all these are auxiliary buffers, malloc'ed in this function
  int32_t *y1 = NULL, *y2 = NULL, *v = NULL, *dv = NULL, *v1 = NULL, *v2 = NULL;
  uint8_t *hash = NULL;
  uint32_t i, norm_v, hash_sz;
  int32_t prod_zv;
  bool b;

  
  if (! bliss_params_init(&p, private_key->kind)) {
    // bad kind/not supported
    return BLISS_B_BAD_ARGS;
  }
  
  a = private_key->a;
  s1 = private_key->s1;
  s2 = private_key->s2;

  n = p.n;

  kappa = p.kappa;

  //opaque, but clearly a pointer type.
  state = init_ntt_state(private_key->kind);
  if (state == NULL) {
    return BLISS_B_NO_MEM;
  }

  
  /* initialize our sampler */
  if (!sampler_init(&sampler, p.sigma, p.ell, p.precision, entropy)) {
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

  dv = calloc(n, sizeof(int32_t));
  if(dv ==  NULL){
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

  // for debugging
  if (false) {
    printf("sign hash\n");
    for (i=0; i<SHA3_512_DIGEST_LENGTH; i++) {
      printf(" %d", hash[i]);
      if (i == 31) printf("\n");
    }
    printf("\n");
    printf("sign: public key\n");
    for (i=0; i<n; i++) {
      printf(" %d", a[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
  }

  /* 1 restart: choose y1, y2 */

 restart:

  for(i = 0; i < n; i++){
    y1[i] = sampler_gauss(&sampler);
    y2[i] = sampler_gauss(&sampler);
  }

  /* 2: compute v = ((2 * xi * a * y1) + y2) mod 2q */
  multiply_ntt(state, v, y1, a);
  
  for (i=0; i<n; i++) {
    // this is v[i] = (2 * v[i] * xi + y2[i]) % q2
    v[i] = smodq(2 * v[i] * p.one_q2 + y2[i], p.q2);
  }

  if (false) {
    printf("sign: v before drop bits\n");
    for (i=0; i<n; i++) {
      printf(" %d", v[i]);
      if ((i & 15) == 15) printf("\n");
    }
  }

  if (false) {
    check_before_drop(private_key, hash, hash_sz, v, y1, y2, &p, state);
  }

  /* 2b: drop bits mod_p */
  assert(check_arg(v, n, p.q2));
  drop_bits(dv, v, n, p.d);
  for (i=0; i<n; i++) {
    dv[i] = smodq(dv[i], p.mod_p);
  }

  /* 3: generateC of v and the hash of the msg */
  if (false) {
    printf("sign: input to generateC\n");
    for (i=0; i<n; i++) {
      printf(" %d", dv[i]);
      if ((i & 31) == 31) printf("\n");
    }
    printf("\n");
  }

  generateC(indices, kappa, dv, n, hash, hash_sz);

  if (false) {
    printf("sign: indices after generateC\n");
    for (i=0; i<kappa; i++) {
      printf(" %d", indices[i]);
    }
    printf("\n\n");
  }

  /* 4: (v1, v2) = greedySC(c) */

  greedy_sc(s1, s2, n, indices, kappa, v1, v2);

  /* 5: choose a random bit b */

  b = entropy_random_bit(entropy);

  /* 6: (z1, z2) = (y1, y2) + (-1)^b * (v1, v2) */

  if(b) {
    for(i = 0; i < n; i++) {
      z1[i] = y1[i] + v1[i];
      z2[i] = y2[i] + v2[i];
    }
  } else {
    for(i = 0; i < n; i++) {
      z1[i] = y1[i] - v1[i];
      z2[i] = y2[i] - v2[i];
    }
  }

  if (false) {
    printf("sign: z1\n");
    for (i=0; i<n; i++) {
      printf(" %d", z1[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
    printf("sign: z2\n");
    for (i=0; i<n; i++) {
      printf(" %d", z2[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n\n");
  }

  /* 7: continue with probability 1/(M exp(-|v|^2/2sigma^2) * cosh(<z, v>/sigma^2)) otherwise restart */
  // NOTE: we could do the ber_exp earlier since it does not depend on z

  norm_v = (uint32_t)(vector_norm2(v1, n) + vector_norm2(v2, n));

  if(p.M <= norm_v) {
    fprintf(stdout, "M = %d norm = %d\n", (int)p.M, (int)norm_v);
  }
  assert(p.M > norm_v);
  
  
  if (! sampler_ber_exp(&sampler, p.M - norm_v)) {
    if(VERBOSE_RESTARTS){ fprintf(stdout, "--> sampler_ber_exp false\n");  }
    goto restart;
  }
  prod_zv = vector_scalar_product(z1, v1, n) + vector_scalar_product(z2, v2, n);
  if (! sampler_ber_cosh(&sampler, prod_zv)) {
    if(VERBOSE_RESTARTS){ fprintf(stdout, "--> sampler_ber_cosh false\n"); }
    goto restart;
  }

  /* 8: z2 = (drop_bits(v) - drop_bits(v - z2)) mod p  */
  for (i=0; i<n; i++) {
    y1[i] = smodq(v[i] - z2[i], p.q2);
  }
  assert(check_arg(v, n, p.q2));
  drop_bits(v, v, n, p.d);   // drop_bits(v)
  assert(check_arg(y1, n, p.q2));
  drop_bits(y1, y1, n, p.d); // drop_bits(v - z2)
  for (i=0; i<n; i++) {
    z2[i] = v[i] - y1[i]; 
    if (z2[i] <  -p.mod_p/2) { 
      z2[i] += p.mod_p;
    } else if (z2[i] >  p.mod_p/2) {
      z2[i] -= p.mod_p;
    }
    assert(-p.mod_p/2 <= z2[i] && z2[i] < p.mod_p/2);
  }
  
  /* 9: seem to also need to check norms akin to what happens in the entry to verify */
  mul2d(y2, z2, n, p.d);
  if (vector_max_norm(z1, n) > p.b_inf) {
    if(VERBOSE_RESTARTS){ fprintf(stdout, "--> norm z1 too high\n"); }
    goto restart;
  }
  if (vector_max_norm(y2, n) > p.b_inf) {
    if(VERBOSE_RESTARTS){ fprintf(stdout, "--> norm y2 too high\n"); }
    goto restart;
  }
  if (vector_norm2(z1,  n) + vector_norm2(y2, n) > p.b_l2){
    if(VERBOSE_RESTARTS){ fprintf(stdout, "--> euclidean norm too high\n"); }
    goto restart;
  }

  /* return (z1, z2, c) */

  if (false) {
    printf("sign: indices after generateC\n");
    for (i=0; i<kappa; i++) {
      printf(" %d", indices[i]);
    }
    printf("\n\n");
  }

  signature->kind = p.kind;
  signature->z1 = z1;
  signature->z2 = z2;
  signature->c = (uint32_t *)indices;

  /* need to free some stuff */

  retval = BLISS_B_NO_ERROR;

  goto cleanup;

 fail:

  secure_free(&z1, n);
  secure_free(&z2, n);
  secure_free((int32_t **)&indices, kappa);

 cleanup:

  free(hash);
  hash = NULL;

  delete_ntt_state(state);

  secure_free(&v, n);
  secure_free(&dv, n);
  secure_free(&y1, n);
  secure_free(&y2, n);
  secure_free(&v1, n);
  secure_free(&v2, n);


  return retval;	
}




int32_t bliss_b_verify(const bliss_signature_t *signature,  const bliss_public_key_t *public_key, const uint8_t *msg, size_t msg_sz){
  bliss_b_error_t retval;
  bliss_param_t p;
  ntt_state_t state;

  // parameters extracted from p: n = size, q = modulus
  uint32_t n, kappa;
  int32_t  q;

  uint32_t i;
  
  int32_t *a, *z1, *z2, *tz2 = NULL, *v = NULL;
  uint32_t *c_indices, *indices = NULL;
  uint32_t idx;

  uint8_t *hash = NULL;
  size_t hash_sz;

  assert(public_key->kind == signature->kind);
  
  if (! bliss_params_init(&p, public_key->kind)) {
    // bad kind/not supported
    return BLISS_B_BAD_ARGS;
  }

  a = public_key->a;

  n = p.n;
  q = p.q;

  kappa = p.kappa;

  z1 = signature->z1;         /* length n */
  z2 = signature->z2;         /* length n */
  c_indices = signature->c;   /* length kappa */

  //opaque, but clearly a pointer type.
  state = init_ntt_state(public_key->kind);
  if (state == NULL) {
    return BLISS_B_NO_MEM;
  }

  tz2 = calloc(n, sizeof(int32_t));
  if(tz2 ==  NULL){
    retval = BLISS_B_NO_MEM;
    goto fail;
  }

  /* first check the norms */

  if (vector_max_norm(z1, n) > p.b_inf){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }

  /* multiply z2 by 2^d */
  mul2d(tz2, z2, n, p.d);

  if(vector_max_norm(tz2, n) > p.b_inf){
    retval = BLISS_B_BAD_DATA;
    goto fail;
  }

  if (vector_norm2(z1, n) + vector_norm2(tz2, n) > p.b_l2){
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

  if (false) {
    printf("verify hash\n");
    for (i=0; i<SHA3_512_DIGEST_LENGTH; i++) {
      printf(" %d", hash[i]);
      if (i == 31) printf("\n");
    }
    printf("\n");
    printf("verify: c_indices\n");
    for (i=0; i<kappa; i++) {
      printf(" %d", c_indices[i]);
    }
    printf("\n");
    printf("verify: z1\n");
    for (i=0; i<n; i++) {
      printf(" %d", z1[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
    printf("verify: z2\n");
    for (i=0; i<n; i++) {
      printf(" %d", z2[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n\n");

    printf("verify: public key\n");
    for (i=0; i<n; i++) {
      printf(" %d", a[i]);
      if ((i & 15) == 15) printf("\n");
    }
    printf("\n");
  }

  /* v = a * z1 */
  multiply_ntt(state, v, z1, a);

  /* v = (1/(q + 2)) * a * z1 */
  for (i = 0; i < n; i++){
    assert(0 <= v[i] && v[i] < q);
    v[i] = smodq(2*v[i]*p.one_q2, p.q2);  
  }

  /* v += (q/q+2) * c */
  for (i = 0; i < kappa; i++) {
    idx = c_indices[i];
    v[idx] = smodq(v[idx] + (q * p.one_q2), p.q2);
  }
  
  if (false) {
    printf("verify: v before drop bits\n");
    for (i=0; i<n; i++) {
      printf(" %d", v[i]);
      if ((i & 15) == 15) printf("\n");
    }
  }

  assert(check_arg(v, n, p.q2));
  drop_bits(v, v, n, p.d);

  /*  v += z_2  mod p. */
  for (i = 0; i < n; i++){
    v[i] += z2[i];
    v[i] = smodq(v[i], p.mod_p);
    if (v[i] < 0){
      v[i] += p.mod_p;
    }

  }

  if (false) {
    printf("verify: input to generateC\n");
    for (i=0; i<n; i++) {
      printf(" %d", v[i]);
      if ((i & 31) == 31) printf("\n");
    }
    printf("\n");
  }
  generateC(indices, kappa, v, n, hash, hash_sz);

  if (false) {
    printf("verify: indices after generateC\n");
    for (i=0; i<kappa; i++) {
      printf(" %d", indices[i]);
    }
    printf("\n");
  }

  retval = BLISS_B_NO_ERROR;

  for (i = 0; i < kappa; i++){
    if (indices[i] != c_indices[i]){
      retval = BLISS_B_VERIFY_FAIL;
      break;
    }
  }


 fail:
  
  delete_ntt_state(state);

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
  assert(signature != NULL);

  free(signature->z1);
  signature->z1 = NULL;

  free(signature->z2);
  signature->z2 = NULL;

  free(signature->c);
  signature->c = NULL;
}
