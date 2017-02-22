#ifndef __BLISS_B_PARAMS__
#define __BLISS_B_PARAMS__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/* Names for the five varieties of bliss-b */
typedef enum { BLISS_B_0, BLISS_B_1, BLISS_B_2, BLISS_B_3, BLISS_B_4 } bliss_kind_t;


/* Rule of Thumb: if it used as a bound  for a for loop, then it should be uint rather than int.
 *
 */

typedef struct {

  bliss_kind_t kind;     /* the kind of bliss-b (i.e. *this* choice of parameters)  */
  int32_t q;             /* field modulus  */
  uint32_t n;            /* ring size (x^n+1)  */
  uint32_t d;            /* bit drop shift  */
  int32_t mod_p;         /* magic modulus  (derived from d) */
  int32_t q2;            /* 2 * field modulus  */
  int32_t q_inv;         /* floor(2^32/q)      */
  int32_t q2_inv;        /* floor(2^32/q2)     */
  int32_t one_q2;        /* 1/(q+2) mod 2q     */
  uint32_t kappa;        /* index vector size  */
  
  uint32_t b_inf;        /* infinite norm  */
  uint32_t b_l2;         /* L2 norm  */

  
  uint32_t nz1;          /* nonzero +-1  */
  uint32_t nz2;          /* nonzero +-2  */
  
  uint32_t sigma;       /* standard deviation  */

  uint32_t M;           /*  M such that  exp( M / 2 * sigma^2) = m, the repetition rate */

  double m;             /* repetition rate  */

  /*
   * Tables for the NTT transform
   */
  
  const int32_t *w;     /* n roots of unity (mod q)  */
  const int32_t *r;     /* w[i]/n (mod q)  */
  /*
   * parameters used by the sampler (in addition to sigma)
   */
  uint32_t ell;         /* number of rows in table for Gaussian sampling */
  uint32_t precision;   /* 8 * number of columns in the table */
  
} bliss_param_t;


extern bool bliss_params_init(bliss_param_t *params, bliss_kind_t kind);

#endif
