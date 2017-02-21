#ifndef PARAMS_H
#define PARAMS_H


/*
 * We are working over ZZ[x] / (x^N +1) 
 * with coefficients modulo Q = 12289.
 * 
 * N = 1024 (or 512)
 * Q = 12289
 *
 */
#define PARAM_N 1024

#define PARAM_Q 12289 

#define PARAM_K 16

#define POLY_BYTES 1792

#define BLISS_SEEDBYTES 32

#define BLISS_SEEDBYTES 32
#define BLISS_RECBYTES 256

#define BLISS_SENDABYTES (POLY_BYTES + BLISS_SEEDBYTES)
#define BLISS_SENDBBYTES (POLY_BYTES + BLISS_RECBYTES)


#endif
