#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "sampler.h"
#include "cpucycles.h"
#include "tests.h"

static sampler_t sampler;

// bernouilli parameters we try
// since we initialize the sampler for 128 bit precision,
// each parameter must be 16 bytes
static const uint8_t zero[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t one_half[16] = {
  128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint8_t one_third[16] = {
  85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85
};

/*
 * Generates ntests samples and compute statistics
 */
static void test_bernoulli(sampler_t *sampler, const uint8_t *p, uint32_t ntests) {
  uint32_t i, count;
  double check;

  assert(ntests > 0);

  count = 0;
  for (i=0; i<ntests; i++) {
    count += sampler_ber(sampler, p);
  }
  check = ((double) count)/ntests;
  printf("successes = %f, failures = %f\n", check, 1 - check);
}

int main(void){
  if (! sampler_init(&sampler, 271, 22, 128, seed)) {
    fprintf(stderr, "error initializing sampler\n");
    return EXIT_FAILURE;
  }

  printf("test1: p = 0.0\n");
  test_bernoulli(&sampler, zero, 2000000);

  printf("\ntest2: p = 0.5\n");
  test_bernoulli(&sampler, one_half, 20000000);

  printf("\ntest3: p = 1/3\n");
  test_bernoulli(&sampler, one_third, 20000000);

  return EXIT_SUCCESS;
}
