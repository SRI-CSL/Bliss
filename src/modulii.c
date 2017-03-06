#include <stdint.h>

#include "modulii.h"

/*
 * https://blogs.msdn.microsoft.com/devdev/2005/12/12/integer-division-by-constants/
 *
 * To avoid division instructions, compilers use the equalities:
 *    x%Q = x - (x/Q) * Q
 *    x/Q = (x * K) >> k
 * for a constant K that's ceiling(2^k/Q). Clang/gcc use
 * k=45 and K=2863078533.
 *
 * We can pick other values for k, since x is between 0 and
 * (Q-1)^2. The following values work.
 *
 *       k |     K
 *   ----------------------
 *      41 |  178942409
 *      42 |  357884817
 *      43 |  715769634
 *      44 | 1431539267
 *      45 | 2863078533

 *  for k = 41 & Q = 7681
 *  K = 286293876
 *  for k = 41 & 2Q 15362 (Q = 7681) 
 *  K = 143146938
 *  for k = 41 & Q = 12289
 *  K = 178942409
 *  for k = 41 & 2Q = 24578 (Q = 12289) 
 *  K = 89471205

 *
 *
 * also need for mod_p  if we do it this way.
 *
 * another way would be to make K and k part of the params struct and
 * pass them is as arguments to the function:
 *
 * uint32_t divq(int32_t x, int32_t q, uint32_t k, uint32_t K) { 
 *    return (((uint64_t) x) * K) >> k;
 * }
 *
 */
static int32_t divq(int32_t x, int32_t q) {
  switch(q){
  case 12289:   return (((uint64_t) x) * 178942409) >> 41;
  case 24578:   return (((uint64_t) x) * 89471205)  >> 41;
  case 7681:    return (((uint64_t) x) * 286293876) >> 41;
  case 15362:   return (((uint64_t) x) * 143146938) >> 41;
  default: return x / q;
  }
  
  return (((uint64_t) x) * 178942409) >> 41;
}

int32_t modq(int32_t x, int32_t q) {
  assert(x < ((q - 1) ^ 2));
  return x - divq(x, q) * q;
}



