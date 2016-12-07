#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "sampler.h"
#include "cpucycles.h"
#include "tests.h"

static sampler_t sampler;

/*
 * Counters for histogram:
 * for -M < i < +M
 * - count[M+i] = number of samples equal to i
 * - count[2M] = number of samples >= M
 * - count[0] = number of samples <= -M
 */
#define M 2048
#define NUM_COUNTERS (2*M+1)
static uint32_t count[NUM_COUNTERS];

/*
 * Generates ntests samples and compute statistics
 */
static void test_gaussian(sampler_t *sampler, uint32_t ntests) {
  uint32_t i, failed;
  int32_t m, p, x;

  assert(ntests > 0);

  for (i=0; i<NUM_COUNTERS; i++) {
    count[i] = 0;
  }
  failed = 0;
  p = 0;
  m = 0;
  for (i=0; i<ntests; i++) {
    if (sampler_gauss(sampler, &x)) {
      if (x > M) {
	x = M; 
      } else if (x < -M) {
	x = -M;
      }
      count[M + x] ++;
      if (x >= p) { 
	p = x+1;
      }
      if (x <= m) {
	m = x;
      }
    } else {
      failed ++;
    }
  }

  if (failed > 0) {
    printf("The sampler failed %"PRIu32" times\n", failed);
    ntests -= failed;
  }

  if (ntests == 0) {
    printf("Can't generate histogram: no data\n");    
  } else {
    printf("Histogram\n");
    for (x=m; x<p; x++) {
      assert(-M <= x && x <= M);
      printf("fraction[%"PRIi32"] = %g\n", x, ((double)count[M+x]) / ntests);
    }
    printf("\n");
  }
}

int main(void){
  printf("\nTesting Gaussian sampling\n");
  if (! sampler_init(&sampler, 271, 22, 128, seed)) {
    fprintf(stderr, "error initializing sampler\n");
    return EXIT_FAILURE;
  }
  test_gaussian(&sampler, 20000000);

  return EXIT_SUCCESS;
}
