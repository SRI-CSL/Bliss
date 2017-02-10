#ifndef __BLISS_B_ERRORS__
#define __BLISS_B_ERRORS__

#include <stdint.h>


typedef enum {

  BLISS_B_NO_ERROR = 0,

  BLISS_B_VERIFY_FAIL = 1,

  BLISS_B_NO_MEM =  -1,

  BLISS_B_BAD_DATA = -2,

  BLISS_B_BAD_ARGS = -3

} bliss_b_error_t;






#endif
