#ifndef __BLISS_B_ERRORS__
#define __BLISS_B_ERRORS__

#include <stdint.h>

/*
 * Error codes:
 * - NO_MEM: malloc failed in some procedure
 * - BAD_DATA: failed signature verification (variant of B_VERIFY_FAIL)
 * - BAD_ARGS: wrong Bliss versin id
 * - RETRY: key gen failed to generate an invertible
 *   polynomial
 */
typedef enum {
  BLISS_B_NO_ERROR = 0,
  BLISS_B_VERIFY_FAIL = 1,
  BLISS_B_NO_MEM =  -1,
  BLISS_B_BAD_DATA = -2,
  BLISS_B_BAD_ARGS = -3,
  BLISS_B_RETRY = - 4
} bliss_b_error_t;


#endif
