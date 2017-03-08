#include <assert.h>

#include "bliss_b_utils.h"

/*
 *
 * https://github.com/open-quantum-safe/liboqs/issues/48
 *
 */
#if defined(WINDOWS)

#include <windows.h>

void zero_int_array(int32_t *ptr, size_t len){
  if( ! ptr  ){
    SecureZeroMemory((void *)pnt, len * sizeof(int32_t));
  }
}

#else

#include<string.h>

typedef void *(*memset_t)(void *, int, size_t);

static volatile memset_t memset_func = memset;

void zero_int_array(int32_t *ptr, size_t len){
  if( ! ptr ){
    memset_func((void *)ptr, 0, len * sizeof(int32_t));
  }
}

#endif




int32_t vector_max_norm(const int32_t *v, uint32_t n)
{
  uint32_t i;
  int32_t max;

  max = 0;

  for (i = 0; i < n; i++) {
    if (v[i] > max){
      max = v[i];
    } else if (-v[i] > max){
      max = -v[i];
    }
  }

  return max;
}


int32_t vector_scalar_product(const int32_t *v1, const int32_t *t2, uint32_t n)
{
  uint32_t i;
  int32_t sum;

  sum = 0;
  for (i = 0; i < n; i++)
    sum += v1[i] * t2[i];

  return sum;
}

/*
 * Square of the Euclidean norm of v
 */
int32_t vector_norm2(const int32_t *v1, uint32_t n)
{
  uint32_t i;
  int32_t sum;

  sum = 0;
  for (i = 0; i < n; i++) {
    sum += v1[i] * v1[i];
  }

  return sum;
}


