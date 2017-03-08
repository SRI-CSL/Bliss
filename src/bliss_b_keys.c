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
#include "ntt_blzzd.h"


/*
   Constructs a random polyomial

   - v: where the random polynomial is stored
   - n: the length of the polynomial
   - nz1: the number of coefficients that are +-1
   - nz2: the number of coefficients that are +-2
   - entropy: an initialized source of randomness

*/
void uniform_poly(int32_t v[], uint32_t n, uint32_t nz1, uint32_t nz2, entropy_t *entropy)
{
  uint32_t i, j;
  uint64_t x;

  for (i = 0; i < n; i++){
    v[i] = 0;
  }
  
  i = 0;
  while (i < nz1) {
    x = entropy_random_uint64(entropy);  
    j = (x >> 1) % n; // nb: uniform because n is a power of 2
    if (v[j] != 0)
      continue;
    v[j] = x & 1 ? 1 : -1;
    i++;
  }

  i = 0;
  while (i < nz2) {
    x = entropy_random_uint64(entropy);
    j = (x >> 1) % n; // nb: uniform because n is a power of 2
    if (v[j] != 0)
      continue;
    v[j] = x & 1 ? 2 : -2;
    i++;
  }
}

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

static int32_t bliss_b_private_key_init(bliss_private_key_t *private_key, bliss_kind_t kind){
  uint32_t n;
  int32_t *f = NULL, *g = NULL, *a = NULL;
  bliss_param_t p;

  if (! bliss_params_init(&p, kind)) {
    // bad kind/not supported
    return BLISS_B_BAD_ARGS;
  }

  n = p.n;

  /* we calloc so we do not have to zero them out later */
  f = calloc(n, sizeof(int32_t));
  if (f == NULL) {
    goto fail;
  }

  /* we calloc so we do not have to zero them out later */
  g = calloc(n, sizeof(int32_t));
  if (g == NULL) {
    goto fail;
  }

  /* we calloc so we do not have to zero them out later */
  a = calloc(n, sizeof(int32_t));
  if (a == NULL) {
    goto fail;
  }

  private_key->kind = kind;
  private_key->s1 = f;
  private_key->s2 = g;
  private_key->a = a;

  return BLISS_B_NO_ERROR;

 fail:

  free(f);
  free(g);
  free(a);

  return BLISS_B_NO_MEM;

}

/**
 * Bliss-b public and sign key generation
 *        sign key is    f, g small and f invertible
 *        public key is  a_q = -(2g-1)/f mod q = (2g'+1)/f
 */
int32_t bliss_b_private_key_gen(bliss_private_key_t *private_key, bliss_kind_t kind, entropy_t *entropy){
  int32_t retcode;
  int32_t i, j, x;
  int32_t *t = NULL, *u = NULL;
  ntt_state_t state;
  bliss_param_t p;

  if (! bliss_params_init(&p, kind)) {
    // bad kind/not supported
    return BLISS_B_BAD_ARGS;
  }

  assert(private_key != NULL);

  retcode = bliss_b_private_key_init(private_key, kind);

  if (retcode !=  BLISS_B_NO_ERROR) {
    return retcode;
  }

  //opaque, but clearly a pointer type.
  state = init_ntt_state(kind);
  if (state == NULL) {
    goto fail;
  }

  t = calloc(p.n, sizeof(int32_t));
  if (t == NULL) {
    goto fail;
  }

  u = calloc(p.n, sizeof(int32_t));
  if (u == NULL) {
    goto fail;
  }

  /* randomize g */
  uniform_poly(private_key->s2, p.n, p.nz1, p.nz2, entropy);

  /* g = 2g - 1 */
  for (i = 0; i < p.n; i++)
    private_key->s2[i] *= 2;
  private_key->s2[0] --;

  /*
  for (i = 0; i < p.n; i++)
    t[i] = private_key->s2[i];
  */

  //N.B. ntt_t t
  forward_ntt(state, t, private_key->s2);
    //ntt32_xmu(t, p.n, p.q, private_key->s2, p.w);
    //ntt32_fft(t, p.n, p.q, p.w);

  /* find an invertible f  */
  for (j = 0; j < 4; j++) {

    /* randomize f  */
    uniform_poly(private_key->s1, p.n, p.nz1, p.nz2, entropy);

    /* a = g/f. Try again if f is not invertible. */
    if(!invert_polynomial(state, u, private_key->s1)){
      continue;
    }
    
    /*  success!  */

    ntt32_xmu(private_key->a, p.n, p.q, t, u);
    ntt32_fft(private_key->a, p.n, p.q, p.w);

    
    ntt32_xmu(private_key->a, p.n, p.q, private_key->a, p.r);

    /* retransform (Saarinen says: can we optimize this?) */
    ntt32_cmu(private_key->a, p.n, p.q, private_key->a, -1);    /* flip sign  IAM&BD: this seems to be wrong. see tests/static/text_blzzd.c */
    ntt32_flp(private_key->a, p.n, p.q);
    ntt32_xmu(private_key->a, p.n, p.q, private_key->a, p.w);
    ntt32_fft(private_key->a, p.n, p.q, p.w);

    
    /* TL:  I just understood, it comes from a slight optimization of BLISS
     *
     *      During the keygen, we set a_1 = 2a_q mod 2q, where a_q = (2g-1)/f mod q
     *      But when we use it, we use (zeta*a_1) mod 2q, where zeta = (q-2)^(-1) mod 2q
     *      Now, zeta*a_1 mod q = -a_q mod q
     *      Therefore that is what they compute in the keygeneration, and I guess they are
     *      doing the computation mod 2q using a "trick" to get the mod 2q in the signing alg.
     *      see https://github.com/mjosaarinen/blzzrd/blob/master/pubpriv.c#L254
     *      (This comes from the fact that you can compute mod 2q by coputing mod q, and then
     *      looking at the result mod 2)
     */

    /* TL:
     *       Now that I read that again, I don't know why it's done this way but I think it's actually
     *       giving a BLISS-B key.
     *
     *       Indeed, a = (2g-1)/f and we compute NTT (-a), i.e. NTT((2*(-g)+1)/f)
     *       Now the distribution of g is centered, therefore it does not matter.
     */

    /*  normalize a */
    for (i = 0; i < p.n; i++) {
      x = private_key->a[i] % p.q;
      if (x < 0)
        x += p.q;
      private_key->a[i] = x;
    }

    secure_free(&t, p.n);
    secure_free(&u, p.n);

    // BD: for debugging
    if (false) {
      check_key(private_key, &p, state);
    }

    

    delete_ntt_state(state);

    return BLISS_B_NO_ERROR;
  }

 fail:

  secure_free(&t, p.n);
  secure_free(&u, p.n);

  delete_ntt_state(state);
  
  bliss_b_private_key_delete(private_key);

  return BLISS_B_NO_MEM;
}

void bliss_b_private_key_delete(bliss_private_key_t *private_key){
  bliss_param_t p;

  assert(private_key != NULL);
  
  if (! bliss_params_init(&p, private_key->kind)) {
    // bad kind/not supported
    return;
  }



  secure_free(&private_key->s1, p.n);
  secure_free(&private_key->s2, p.n);
  secure_free(&private_key->a, p.n);

}


int32_t bliss_b_public_key_extract(bliss_public_key_t *public_key, const bliss_private_key_t *private_key){
  uint32_t n, i;
  int32_t *a, *b;

  assert(private_key != NULL && private_key->a != NULL);

  bliss_param_t p;

  
  if (! bliss_params_init(&p, private_key->kind)) {
    // bad kind/not supported
    return BLISS_B_BAD_ARGS;
  }

  n = p.n;

  b = private_key->a;

  /* we calloc so we do not have to zero them out later */
  a = calloc(n, sizeof(int32_t));
  if (a == NULL) {
    return BLISS_B_NO_MEM;
  }

  for(i = 0; i < n; i++){
    a[i] = b[i];
  }

  public_key->kind = private_key->kind;
  public_key->a = a;

  return BLISS_B_NO_ERROR;

}


void bliss_b_public_key_delete(bliss_public_key_t *public_key){

  assert(public_key != NULL);

  free(public_key->a);
  public_key->a = NULL;
}
