#ifndef _BLISS_B_UTILS_H
#define _BLISS_B_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


/*
 *  Zeros len bytes of ptr, designed in such a way as to NOT be
 *  optimized out by compilers. If the ptr is NULL, the operation
 *  is a noop.
 *   - ptr, pointer to bytes to be zeroed.
 *   - len, the number of bytes to be zeroed.
 */
extern void zero_memory(void *ptr, size_t len);

/*
 * GreedySC blzzd version
 *
 * should be static once we choose one and use it.
 *
 * BD: yay!
 */
extern void greedy_sc(const int32_t *s1, const int32_t *s2, int32_t n,  const int32_t *c_indices, int32_t kappa, int32_t *v1, int32_t *v2);


/*
 * Computes the max norm of a vector of a given length.
 * - v a vector of length n
 * - n the length
 *
 * returns the componentwise max
 */
extern int32_t vector_max_norm(const int32_t *v, int32_t n);

/*
 * Computes the scalar product of two vectors of a given length.
 * - v1 a vector of length n
 * - v2 a vector of length n
 * - n the length
 *
 * returns the scalar product.
 */
extern int32_t vector_scalar_product(const int32_t *v1, const int32_t *v2, int32_t n);

/*
 * Square of the Euclidean norm of v1
 */
extern uint32_t vector_norm2(const int32_t *v1, uint32_t n);


#endif
