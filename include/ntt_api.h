#ifndef __NTT_API_H
#define __NTT_API_H

#include <stdbool.h>
#include <stdint.h>

#include "bliss_b_params.h"


/*
 *
 * Libraryish API for our uses of the Number Theoretic Transform.
 *
 *
 */

typedef int32_t *polynomial_t;

typedef void *ntt_state_t;

typedef void *ntt_t;   //might be better to bite the bullet and admit it is int32_t*


extern ntt_state_t init_ntt_state(bliss_kind_t kind);

extern void delete_ntt_state(ntt_state_t state);


extern  ntt_t init_ntt(ntt_state_t state);

extern void delete_ntt(ntt_state_t state, ntt_t input);



extern void forward_ntt(const ntt_state_t state, ntt_t output, const polynomial_t input);

extern void inverse_ntt(const ntt_state_t state, polynomial_t output, const ntt_t input);

extern void negate_ntt(const ntt_state_t state, ntt_t inplace);

extern void product_ntt(const ntt_state_t state, ntt_t output, const ntt_t lhs,  const ntt_t rhs);

extern bool invert_polynomial(const ntt_state_t state, ntt_t output, const polynomial_t input);



/*
 *
 * Multiplies lhs by rhs and places the result in result.
 *
 *  -- lhs is a polynomial of degree n.
 *  -- rhs is an ntt of a polynomial of degree n.
 *
 * returns a polynomial of degree n, whose int32_t coeffs are in [0, q)
 *
 */
static inline void multiply_ntt(const ntt_state_t state, polynomial_t result, polynomial_t lhs, ntt_t rhs){

  ntt_t temp = init_ntt(state);

  forward_ntt(state, temp, lhs);

  product_ntt(state, temp, temp,  rhs);

  inverse_ntt(state, result, temp);

  delete_ntt(state, temp);

}




#endif
