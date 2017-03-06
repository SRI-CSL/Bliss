#ifndef __NTT_API_H
#define __NTT_API_H

#include <stdbool.h>
#include <stdint.h>


/*
 * Libraryish API for our uses on the Number Theoretic Transform.
 *
 *
 */

typedef int32_t* polynomial_t;

typedef void * ntt_state_t;

typedef void * ntt_t;

extern void init_ntt_state(ntt_state_t* state, bliss_kind_t k);

extern void delete_ntt_state(ntt_state_t* state);


extern void forward_ntt(const ntt_state_t* state, ntt_t* output, const polynomial_t *input);

extern void inverse_ntt(const ntt_state_t* state, polynomial_t *output, const ntt_t *input);

extern void negate_ntt(const ntt_state_t* state, ntt_t *inplace);

extern void product_ntt(const ntt_state_t* state, ntt_t *output, const ntt_t *lhs,  const ntt_t *rhs);

extern bool invert_polynomial(const ntt_state_t* state, ntt_t *output, const polynomial_t *input);


#endif
