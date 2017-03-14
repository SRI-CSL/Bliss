
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "ntt_api.h"
#include "bliss_b_params.h"
#include "ntt_blzzd.h"

/*
 *
 * Implementation of our NTT API using ntt_blzzd 
 *
 *
 *
 * typedef int32_t* polynomial_t;
 *
 * typedef void* ntt_state_t;
 *
 * typedef void* ntt_t;
 *
 */


typedef struct {
  int32_t  q;             /* field modulus  */
  uint32_t n;             /* ring size (x^n+1)  */
  const int32_t *w;       /* n roots of unity (mod q)  */
  const int32_t *r;       /* w[i]/n (mod q)  */
} ntt_state_simple_t;


ntt_state_t init_ntt_state(bliss_kind_t kind){
  ntt_state_simple_t *s;
  bliss_param_t p;

  if (! bliss_params_init(&p, kind)) {
    return NULL;
  }

  s = malloc(sizeof(ntt_state_simple_t));

  if (s != NULL) {
    s->q = p.q;
    s->n = p.n;
    s->w = p.w;
    s->r = p.r;
  }
  
  return (ntt_state_t)s;
}

void delete_ntt_state(ntt_state_t state){
  assert(state != NULL);
  free(state);
}


ntt_t init_ntt(ntt_state_t state){
  ntt_state_simple_t *s = (ntt_state_simple_t *)state;
  int32_t* ntt;
  assert(state != NULL);
  
  ntt = calloc(s->n, sizeof(int32_t));

  return (ntt_t)ntt;
}

void delete_ntt(ntt_state_t state, ntt_t input){
  assert(state != NULL);
  assert(input != NULL);
  free(input);

}



void forward_ntt(const ntt_state_t state, ntt_t output, const polynomial_t input){
  ntt_state_simple_t *s = (ntt_state_simple_t *)state;
  assert(state != NULL);

  ntt32_xmu(output, s->n, s->q, input, s->w);         /* multiply by powers of psi                  */
  ntt32_fft(output, s->n, s->q, s->w);                /* result = ntt(input)                        */

}

void inverse_ntt(const ntt_state_t state, polynomial_t output, const ntt_t input){
  ntt_state_simple_t *s = (ntt_state_simple_t *)state;
  uint32_t i;
  int32_t *a = (int32_t *)input;
  assert(state != NULL);

  for(i = 0; i < s->n; i++){
    output[i] = a[i];
  }
  
  ntt32_fft(output, s->n, s->q, s->w);             /* result = ntt(input) = inverse ntt(poly) modulo reordering (input = ntt(poly)) */
  ntt32_xmu(output, s->n, s->q, output, s->r);     /* multiply by powers of psi^-1  */
  ntt32_flp(output, s->n, s->q);                   /* reorder: result mod q */

}

void negate_ntt(const ntt_state_t state, ntt_t inplace){
  ntt_state_simple_t *s = (ntt_state_simple_t *)state;
  int32_t *result = (int32_t *)inplace;
  assert(state != NULL);

  ntt32_cmu(result, s->n, s->q, result, -1);

}

void product_ntt(const ntt_state_t state, ntt_t output, const ntt_t lhs,  const ntt_t rhs){
  ntt_state_simple_t *s = (ntt_state_simple_t *)state;
  int32_t *a = lhs;
  int32_t *b = rhs;
  int32_t *result = output;

  assert(state != NULL);

  ntt32_xmu(result, s->n, s->q, a, b);       /* result = lhs * rhs (pointwise product) */

}

bool invert_polynomial(const ntt_state_t state, ntt_t output, const polynomial_t input){
  ntt_state_simple_t *s = (ntt_state_simple_t *)state;
  int32_t *a = output;
  uint32_t i;
  int32_t x;

  assert(state != NULL);

  forward_ntt(state, output, input);
  for (i = 0; i < s->n; i++) {
    x = a[i];
    if (x == 0) return false;           /* not invertible */
    x = ntt32_pwr(x, s->q - 2, s->q);   /* x^(q-2) = inverse of x */
    a[i] = x;
  }

  return true;
}
