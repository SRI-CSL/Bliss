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




/*
 * GreedySC version (based on blzzd)
 *
 * should be static once we choose one and use it.
 *
 */
void greedy_sc(const int32_t *s1, const int32_t *s2, uint32_t n,  const uint32_t *c_indices, uint32_t kappa, int32_t *v1, int32_t *v2)
{
  uint32_t index, i, k;
  int32_t sign;

  for (i = 0; i < n; i++) {
    v1[i] = 0;
    v2[i] = 0;
  }

  for (k = 0; k < kappa; k++) {

    index = c_indices[k];
    sign = 0;
    /* \xi_i = sign(<v, si>) */
    for (i = 0; i < n - index; i++) {
      sign += s1[i] * v1[index + i] + s2[i] * v2[index + i];
    }
    for (i = n - index; i < n; i++) {
      sign -= s1[i] * v1[index + i - n] + s2[i] * v2[index + i - n];
    }
    /* v = v - \xi_i . si */
    if (sign > 0) {
      for (i = 0; i < n - index; i++) {
	v1[index + i] -= s1[i];
	v2[index + i] -= s2[i];
      }
      for (i = n - index; i < n; i++) {
	v1[index + i - n] += s1[i];
	v2[index + i - n] += s2[i];
      }
    } else {
      for (i = 0; i < n - index; i++) {
	v1[index + i] += s1[i];
	v2[index + i] += s2[i];
      }
      for (i = n - index; i < n; i++) {
	v1[index + i - n] -= s1[i];
	v2[index + i - n] -= s2[i];
      }
    }
  }
}


int32_t vector_max_norm(const int32_t *v, uint32_t n)
{
  uint32_t i;
  int32_t max;

  max = 0;
  for (i = 0; i < n; i++) {
    if (v[i] > max)
      max = v[i];
    if (-v[i] > max)
      max = -v[i];
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


