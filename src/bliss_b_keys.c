#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "bliss_b_errors.h"
#include "bliss_b_params.h"
#include "bliss_b_keys.h"
#include "bliss_b_utils.h"
#include "entropy.h"

#include "ntt_api.h"

/*
   Constructs a random polyomial

   - v: where the random polynomial is stored
   - n: the length of the polynomial
   - nz1: the number of coefficients that are +-1
   - nz2: the number of coefficients that are +-2
   - entropy: an initialized source of randomness

*/
void uniform_poly(int32_t v[], uint32_t n, uint32_t nz1, uint32_t nz2, entropy_t *entropy) {
  uint32_t i, j;
  uint16_t x;
  int32_t mask;

  for (i = 0; i < n; i++){
    v[i] = 0;
  }

  i = 0;
  while (i < nz1) {
    x = entropy_random_uint16(entropy);
    j = (x >> 1) % n;               // nb: uniform because n is a power of 2
    mask = -(1^(v[j]&1));           // mask = 1...1 if v[j] == 0 else 0
    i += mask&1;                    // add 1 only if v[j] == 0
    v[j] += (-1 + ((x&1)<<1))&mask; // v[j] = -1 if x&1 == 0 else 1
  }

  i = 0;
  while (i < nz2) {
    x = entropy_random_uint16(entropy);
    j = (x >> 1) % n;                     // nb: uniform because n is a power of 2
    mask = -(1^((v[j]&1)|((v[j]&2)>>1))); // mask = 1...1 if v[j] == 0 or v[j] == 1 else 0
    i += mask&1;                          // add 1 only if v[j] == 0 or v[j] == 1
    v[j] += (-2 + ((x&1)<<2))&mask;       // v[j] = -2 if x&1 == 0 else 2
  }
}

#if 0
// BD: for debugging
static void check_key(bliss_private_key_t *key, bliss_param_t *p, ntt_state_t state) {
  int32_t aux[512];
  uint32_t i, n;
  int32_t q;

  assert(key->kind == p->kind);

  n = p->n;
  q = p->q;

  // compute product key->s1 * key->a
  multiply_ntt(state, aux, key->s1, key->a);

  printf("a * s1:\n");
  for (i=0; i<n; i++) {
    printf(" %d", aux[i]);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n");

  printf("s1:\n");
  for (i=0; i<n; i++) {
    printf(" %d", key->s1[i]);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n");

  printf("s2:\n");
  for (i=0; i<n; i++) {
    printf(" %d", key->s2[i]);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n");

  printf("2 * (a * s1)[i] + (q + 2) * s2[i] mod q\n");
  for (i=0; i<n; i++) {
    printf(" %d", (2 * aux[i] + (q + 2) * key->s2[i]) % q);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n");

  printf("2 * (a * s1)[i] + (q + 2) * s2[i] mod 2\n");
  for (i=0; i<n; i++) {
    printf(" %d", (2 * aux[i] + (q + 2) * key->s2[i]) % 2);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n");

  printf("2 * (a * s1)[i] + (q + 2) * s2[i] mod 2q:\n");
  for (i=0; i<n; i++) {
    printf(" %d", (2 * aux[i] + (q + 2) * key->s2[i]) % p->q2);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n");

  printf("2 * zeta * (a * s1)[i] + s2[i] mod 2q:\n");
  for (i=0; i<n; i++) {
    printf(" %d", (2 * p->one_q2 * aux[i] + key->s2[i]) % p->q2);
    if ((i & 15) == 15) printf("\n");
  }
  printf("\n\n");
}
#endif

/*
 * Initialize private_key:
 * - kind = Bliss B variant
 * - n = corresponding key size (ring size)
 *
 * Result
 * - kind and n are stored
 * - buffers for s1, s2 and a are allocated and
 *   initialized to all zeros
 *
 * Error code:
 * - if all works right, result = BLISS_B_NO_ERROR
 * - if kind is no supported, result = BLISS_B_BAD_ARGS
 * - if malloc fails, result = BLISS_B_NO_MEM
 *
 * If the result is anything other than BLISS_B_NO_ERROR then private_key
 * is unchanged.
 */
static int32_t bliss_b_private_key_init(bliss_private_key_t *private_key, bliss_kind_t kind, uint32_t n){
  int32_t *f, *g, *a;

  /* we calloc so we do not have to zero them out later */
  f = NULL;
  g = NULL;
  a = NULL;
  f = calloc(n, sizeof(int32_t));
  g = calloc(n, sizeof(int32_t));
  a = calloc(n, sizeof(int32_t));
  if (f == NULL || g == NULL || a == NULL) {
    goto fail;
  }

  private_key->kind = kind;
  private_key->n = n;
  private_key->s1 = f;
  private_key->s2 = g;
  private_key->a = a;

  return BLISS_B_NO_ERROR;

 fail:
  /* calloc failed */
  free(f);
  free(g);
  free(a);

  return BLISS_B_NO_MEM;

}


/*
 * Delete private key
 * - it must have been initialized by the previous function
 */
void bliss_b_private_key_delete(bliss_private_key_t *private_key){
  uint32_t n;

  n = private_key->n;
  secure_free(&private_key->s1, n);
  secure_free(&private_key->s2, n);
  secure_free(&private_key->a, n);
}

/*
 * Bliss-b public and sign key generation
 * the sign key is a pair f, g small and f invertible
 * the public key is  a_q = -(2g-1)/f mod q = (2g'+1)/f
 *
 * Result:
 * - if all goes well, the function returns BLISS_B_NO_ERROR
 *   private_key->kind is set to kind
 *   f is stored in private_key->s1,
 *   g is stored in private_key->s2,
 *   NTT(a_q) is stored in private_key->a
 *
 * Error codes:
 * - BLISS_B_BAD_ARGS: kind is not supported
 * - BLISS_B_NO_MEM: failed to allocate buffers
 * - BLISS_B_RETRY: failed to construct an invertible f after 10 attempts
 *
 */
int32_t bliss_b_private_key_gen(bliss_private_key_t *private_key, bliss_kind_t kind, entropy_t *entropy){
  int32_t retcode;
  int32_t i, j;
  int32_t *t, *u;
  ntt_state_t state;
  bliss_param_t p;

  if (! bliss_params_init(&p, kind)) {
    // not supported
    return BLISS_B_BAD_ARGS;
  }

  /* Allocate buffers */
  retcode = bliss_b_private_key_init(private_key, kind, p.n);
  if (retcode != BLISS_B_NO_ERROR) {
    return retcode;
  }

  /* Auxiliary buffers and ntt state */
  t = NULL;
  u = NULL;
  state = init_ntt_state(kind);
  t = calloc(p.n, sizeof(int32_t));
  u = calloc(p.n, sizeof(int32_t));
  if (state == NULL || t == NULL || u == NULL) {
    retcode = BLISS_B_NO_MEM;
    goto fail;
  }

  /* random g */
  uniform_poly(private_key->s2, p.n, p.nz1, p.nz2, entropy);

  /* g = 2g - 1   N.B the Bliss-B paper uses 2g + 1 */
  for (i = 0; i < p.n; i++)
    private_key->s2[i] *= 2;
  private_key->s2[0] --;

  /* find an invertible f: try 10 times */
  for (j = 0; j < 10; j++) {
    /* pick a random f then check if it's invertible */
    uniform_poly(private_key->s1, p.n, p.nz1, p.nz2, entropy);
    if (invert_polynomial(state, u, private_key->s1)) {
      /*
       * Success:
       * - u contains NTT f^-1
       * - compute a = - (2g - 1)/f = - s2/s1
       */
      forward_ntt(state, t, private_key->s2);   // t := NTT(2g - 1)
      product_ntt(state, private_key->a, t, u); // a := NTT((2g - 1)/f)
      negate_ntt(state, private_key->a);        // a := NTT( - (2g - 1)/f)

#if 0
      // BD: for debugging (iam: must do it before cleanup)
      check_key(private_key, &p, state);
#endif

      /* Cleanup */
      secure_free(&t, p.n);
      secure_free(&u, p.n);
      delete_ntt_state(state);

      return BLISS_B_NO_ERROR;
    }
  }

  // Unlucky: didn't find an invertible f in 10 tries
  retcode = BLISS_B_RETRY;

 fail:
  secure_free(&t, p.n);
  secure_free(&u, p.n);
  delete_ntt_state(state);
  bliss_b_private_key_delete(private_key);

  return retcode;
}

/*
 * Copy private_key->a into public_key
 * - return BLISS_B_NO_MEM if malloc fails
 * - return BLISS_B_NO_ERROR otherwise
 */
int32_t bliss_b_public_key_extract(bliss_public_key_t *public_key, const bliss_private_key_t *private_key){
  uint32_t n, i;
  int32_t *a, *b;
  int32_t retcode;

  n = private_key->n;
  b = private_key->a;

  /* we calloc so we do not have to zero it out later */
  retcode = BLISS_B_NO_MEM;
  a = calloc(n, sizeof(int32_t));
  if (a != NULL) {
    retcode = BLISS_B_NO_ERROR;
    for(i = 0; i < n; i++){
      a[i] = b[i];
    }
  }

  /* if malloc fails, we set public_key->a to NULL */
  public_key->kind = private_key->kind;
  public_key->n = n;
  public_key->a = a;

  return retcode;
}


void bliss_b_public_key_delete(bliss_public_key_t *public_key){
  free(public_key->a);
  public_key->a = NULL;
}
