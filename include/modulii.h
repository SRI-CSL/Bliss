#ifndef __MODULII__
#define __MODULII__

#include <assert.h>
#include <stdint.h>

/*
 * Simple implementation of modq
 * (could optimize, cf. tests/static/mod.c)
 */
static inline int32_t smodq(int32_t x, int32_t q){
  assert(q > 0);
  int32_t y = x % q;
  return y + ((y >> 31) & q); 
}

#endif
