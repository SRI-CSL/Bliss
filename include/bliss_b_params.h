#ifndef __BLISS_B_PARAMS__
#define __BLISS_B_PARAMS__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/* Names for the five varieties of bliss-b */
typedef enum { BLISS_B_0, BLISS_B_1, BLISS_B_2, BLISS_B_3, BLISS_B_4 } bliss_kind_t;


/*
 * Rule of Thumb: if it used as a bound for a for loop, then it should be uint rather than int.
 * But we keep the modulii related parameters as signed since they are used as operands to * and % 
 * with other signed values as operands, and so we do not want their unsignedness to corrupt 
 * the signed values.
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

  
  uint32_t nz1;          /* nonzero +-1  aka delta_1*n in L Ducas' Bliss-B paper */
  uint32_t nz2;          /* nonzero +-2  aka delta_2*n  in L Ducas' Bliss-B paper */
  
  uint32_t sigma;        /* standard deviation  */

  uint32_t M;            /*  We use P_{max} given on page 7 of L Ducas' Bliss-B  paper */

  double m;              /* repetition rate  */

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



/*
 * Why we use M = p_max (by Tancrede)
 *
 * 
 * 
 * Let's go:
 * 
 * It should always hold that p->M >= norm_v in order to ensure that
 * 1/(M*exp(-||v||^2/(2*sigma^2))) <= 1
 * 
 * BLISS-B states that ||v||^2 < P_max, where P_max is given on p.7
 * 
 * *** Therefore you see here that you need p->M = P_max. ***
 * 
 * 
 * Let's take BLISS-B1
 * P_max = 17825
 * Which makes exp(-17825/(2*215^2)) = 0.6800330729375813
 * and a repetition rate m that needs to verify m >= 1.21264863594269231
 * which yields alpha = sqrt(1/(2*log(m))) = 1.610362655
 * 
 * The values M and alpha from BLISS-B are actually not parameters, 
 * they are approximate values to understand what is going on.
 * 
 * If you had taken alpha=1.610, we would have 
 * exp(1/(2*1.61*1.61)) = 1.2127539833
 * and if we compute p->M as you did, it now yields
 * p->M = ((2*sigma^2)/(2*alpha^2)) = (sigma/alpha)^2 = 17833.031 (we need to take the ceil)
 * (this is where BLISS-B error in the paper is:
 * alpha = sigma / sqrt(P_max)) and not alpha = sigma/P_max)
 * 
 * 
 * 
 * Anyway, the *real* value one should be taking for p->M is P_max
 * computed as in BLISS-B p.7
 * 
 */

#endif


