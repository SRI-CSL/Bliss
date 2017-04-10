#ifndef _BLISS_B_UTILS_H
#define _BLISS_B_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


/*
 *  Zeros len bytes of a int32_t array ptr, designed in such a way as to NOT be
 *  optimized out by compilers. If the ptr is NULL, the operation
 *  is a noop.
 *   - ptr, pointer to int32_t to be zeroed.
 *   - len, the number of int32_t to be zeroed.
 */
extern void zero_int_array(int32_t *ptr, size_t len); 

static inline void secure_free(int32_t **ptr_p, size_t len){
  zero_int_array(*ptr_p, len);
  free(*ptr_p);
  *ptr_p = NULL;
}




/*
 * Computes the max norm of a vector of a given length.
 * - v a vector of length n
 * - n the length
 *
 * returns the componentwise max
 */
extern int32_t vector_max_norm(const int32_t *v, uint32_t n);

/*
 * Computes the scalar product of two vectors of a given length.
 * - v1 a vector of length n
 * - v2 a vector of length n
 * - n the length
 *
 * returns the scalar product (ignore overflows).
 */
extern int32_t vector_scalar_product(const int32_t *v1, const int32_t *v2, uint32_t n);

/*
 * Square of the Euclidean norm of v (ignore overflows)
 */
extern int32_t vector_norm2(const int32_t *v, uint32_t n);


#endif
