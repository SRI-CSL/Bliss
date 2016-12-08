#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "sampler.h"
#include "cpucycles.h"
#include "tests.h"

// hard-coded seed for testing
static uint8_t seed[SHA3_512_DIGEST_LENGTH] = {
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
  0, 1, 2, 3, 4, 5, 6, 7,
};


static sampler_t sampler;

/*
 * Counters for histogram
 * - count[i] = number of samples equal to i (for i < NUM_COUNTERS)
 * - count[NUM_COUNTER] = number of samples >= NUM_COUNTERS
 */
#define NUM_COUNTERS 16
static uint32_t count[NUM_COUNTERS];

/*
 * Generates ntests samples and compute statistics
 * - count[i] = number of samples equal to i
 */
static void test_pos_binary(sampler_t *sampler, uint32_t ntests) {
  uint32_t i, x, failed, p;

  assert(ntests > 0);

  for (i=0; i<NUM_COUNTERS; i++) {
    count[i] = 0;
  }
  failed = 0;
  p = 0;
  for (i=0; i<ntests; i++) {
    if (sampler_pos_binary(sampler, &x)) {
      if (x >= NUM_COUNTERS) {
	x = NUM_COUNTERS - 1;
      }
      if (x >= p) { 
	p = x+1;
      }
      count[x] ++;
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
    for (i=0; i<p; i++) {
      printf("fraction[%"PRIu32"] = %g\n", i, ((double)count[i]) / ntests);
    }
    printf("\n");
  }
}

int main(void){
  printf("\nTesting pos binary sampling\n");
  if (! sampler_init(&sampler, 271, 22, 128, seed)) {
    fprintf(stderr, "error initializing sampler\n");
    return EXIT_FAILURE;
  }
  test_pos_binary(&sampler, 20000000);

  return EXIT_SUCCESS;
}
