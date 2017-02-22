#include <assert.h>

#include "bliss_b_utils.h"

/*
 *
 * https://github.com/open-quantum-safe/liboqs/issues/48
 *
 */
#if defined(WINDOWS)

#include <windows.h>

void zero_memory(void *ptr, size_t len){
  if( ! ptr  ){
    SecureZeroMemory(pnt, len);
  }
}

#else

#include<string.h>

typedef void *(*memset_t)(void *, int, size_t);

static volatile memset_t memset_func = memset;

void zero_memory(void *ptr, size_t len){
  if( ! ptr ){
    memset_func(ptr, 0, len);
  }
}

#endif




/**
 * GreedySC strongswan version
 *
 * should be static once we choose one and use it.
 *
 * BD: meh.
 */
void greedy_sc_strongswan(const int32_t *s1, const int32_t *s2, int32_t n, const uint32_t *c_indices, int32_t kappa, int32_t *v1, int32_t *v2)
{
  int32_t index, i, k, sign;

  for (i = 0; i < n; i++)
    {
      v1[i] = 0;
      v2[i] = 0;
    }
  for (k = 0; k < kappa; k++) {
    index = c_indices[k];
    sign = 0;
    /* \xi_i = sign(<v, si>) */
    for (i = 0; i < index; i++) {
      sign -= (v1[i] * s1[i - index + n] + v2[i] * s2[i - index + n]);
    }
    for (i = index; i < n; i++)	{
      sign += (v1[i] * s1[i - index] + v2[i] * s2[i - index]);
    }
    /* v = v - \xi_i . si */
    for (i = 0; i < index; i++) {
      if (sign > 0) {
	v1[i] += s1[i - index + n];
	v2[i] += s2[i - index + n];
      } else {
	v1[i] -= s1[i - index + n];
	v2[i] -= s2[i - index + n];
      }
    }
    for (i = index; i < n; i++)	{
      if (sign > 0) {
	v1[i] -= s1[i - index];
	v2[i] -= s2[i - index];
      } else {
	v1[i] += s1[i - index];
	v2[i] += s2[i - index];
      }
    }
  }
}


/*
 * GreedySC blzzd version
 *
 * should be static once we choose one and use it.
 *
 * BD: yay!
 */
void greedy_sc_blzzd(const int32_t *s1, const int32_t *s2, int32_t n,  const int32_t *c_indices, int32_t kappa, int32_t *v1, int32_t *v2)
{
  int32_t index, i, k, sign;

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


int32_t vector_max_norm(const int32_t *v, int32_t n)
{
  int32_t i, max;

  max = 0;
  for (i = 0; i < n; i++) {
    if (v[i] > max)
      max = v[i];
    if (-v[i] > max)
      max = -v[i];
  }

  return max;
}


int32_t vector_scalar_product(const int32_t *v1, const int32_t *t2, int32_t n)
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
uint32_t vector_norm2(const int32_t *v1, uint32_t n)
{
  uint32_t i, sum;

  sum = 0;
  for (i = 0; i < n; i++) {
    sum += v1[i] * v1[i];
  }

  return sum;
}


