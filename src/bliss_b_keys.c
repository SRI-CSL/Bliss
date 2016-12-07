#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

#include "bliss_b_errors.h"
#include "bliss_b_params.h"
#include "bliss_b_keys.h"
#include "entropy.h"
#include "polynomial.h"

#include "ntt_blzzd.h"



static inline int32_t bliss_b_private_key_init(bliss_private_key_t *private_key, bliss_kind_t kind){
  int32_t n;
  int32_t *f = NULL, *g = NULL, *a = NULL;
  const bliss_param_t *p;

  p = &bliss_b_params[kind];

  n = p->n;

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

  private_key->p = *p;
  private_key->f = f;
  private_key->g = g;
  private_key->a = a;

  return BLISS_B_NO_ERROR;

 fail:

  free(f);
  free(g);
  free(a);

  return BLISS_B_NO_MEM;

}


int32_t bliss_b_private_key_gen(bliss_private_key_t *private_key, bliss_kind_t kind, entropy_t *entropy){
  int32_t retcode;
  int32_t i, j, x;
  int32_t *t = NULL, *u = NULL;

  bliss_param_t *p;

  assert(private_key != NULL);


  retcode = bliss_b_private_key_init(private_key, kind);

  if (retcode !=  BLISS_B_NO_ERROR) {
    return retcode;
  }

  p = &private_key->p;


  t = calloc(p->n, sizeof(int32_t));
  if (t == NULL) {
    goto fail;
  }

  u = calloc(p->n, sizeof(int32_t));
  if (u == NULL) {
    goto fail;
  }



  /* randomize g */
  uniform_poly(private_key->g, p->n, p->nz1, p->nz2, false, entropy);

  /* g = 2g - 1 */
  for (i = 0; i < p->n; i++)
    private_key->g[i] *= 2;
  private_key->g[0]--;

  for (i = 0; i < p->n; i++)
    t[i] = private_key->g[i];

  ntt32_xmu(t, p->n, p->q, t, p->w);
  ntt32_fft(t, p->n, p->q, p->w);

  /* find an invertible f  */
  for (j = 0; j < 4; j++) {

    /* randomize f  */
    uniform_poly(private_key->f, p->n, p->nz1, p->nz2, j != 0, entropy);

    /* a = g/f. Try again if f is not invertible. */
    for (i = 0; i < p->n; i++)
      u[i] = private_key->f[i];
    ntt32_xmu(u, p->n, p->q, u, p->w);
    ntt32_fft(u, p->n, p->q, p->w);

    for (i = 0; i < p->n; i++) {
      x = u[i] % p->q;
      if (x == 0)
        break;
      x = ntt32_pwr(x, p->q - 2, p->q);
      u[i] = x;
    }
    if (i < p->n)
      continue;

    /*  success!  */
    ntt32_xmu(private_key->a, p->n, p->q, t, u);
    ntt32_fft(private_key->a, p->n, p->q, p->w);
    ntt32_xmu(private_key->a, p->n, p->q, private_key->a, p->r);

    /* retransform (Saarinen says: can we optimize this?) */
    ntt32_cmu(private_key->a, p->n, p->q, private_key->a, -1);    /* flip sign */
    ntt32_flp(private_key->a, p->n, p->q);
    ntt32_xmu(private_key->a, p->n, p->q, private_key->a, p->w);
    ntt32_fft(private_key->a, p->n, p->q, p->w);

    /*  normalize a */
    for (i = 0; i < p->n; i++) {
      x = private_key->a[i] % p->q;
      if (x < 0)
        x += p->q;
      private_key->a[i] = x;
    }

    free(t);
    free(u);

    return BLISS_B_NO_ERROR;
  }

 fail:

  free(t);
  free(u);
  bliss_b_private_key_delete(private_key);

  return BLISS_B_NO_MEM;
}

/* IAM2BD&TL: Should we zero out the memory to be secure? */
void bliss_b_private_key_delete(bliss_private_key_t *private_key){

  assert(private_key != NULL);

  free(private_key->f);
  private_key->f = NULL;

  free(private_key->g);
  private_key->g = NULL;

  free(private_key->a);
  private_key->a = NULL;
}


int32_t bliss_b_public_key_extract(bliss_public_key_t *public_key, const bliss_private_key_t *private_key){
  int32_t n, i;
  int32_t *a, *b;
  const bliss_param_t *p;

  assert(private_key != NULL && private_key->a != NULL);


  p = &private_key->p;

  n = p->n;

  b = private_key->a;

  /* we calloc so we do not have to zero them out later */
  a = calloc(n, sizeof(int32_t));
  if (a == NULL) {
    return BLISS_B_NO_MEM;
  }

  for(i = 0; i < n; i++){
    a[i] = b[i];
  }

  public_key->p = *p;

  return BLISS_B_NO_ERROR;

}


void bliss_b_public_key_delete(bliss_public_key_t *public_key){

  assert(public_key != NULL);

  free(public_key->a);
  public_key->a = NULL;
}
