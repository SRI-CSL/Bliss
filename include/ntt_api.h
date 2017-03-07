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

typedef void *ntt_t;


extern ntt_state_t init_ntt_state(bliss_kind_t kind);

extern void delete_ntt_state(ntt_state_t state);


extern  ntt_t init_ntt(ntt_state_t state);

extern void delete_ntt(ntt_state_t state, ntt_t input);



extern void forward_ntt(const ntt_state_t state, ntt_t output, const polynomial_t input);

extern void inverse_ntt(const ntt_state_t state, polynomial_t output, const ntt_t input);

extern void negate_ntt(const ntt_state_t state, ntt_t inplace);

extern void product_ntt(const ntt_state_t state, ntt_t output, const ntt_t lhs,  const ntt_t rhs);

extern bool invert_polynomial(const ntt_state_t state, ntt_t output, const polynomial_t input);


#endif
