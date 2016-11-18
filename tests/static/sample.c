#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "sampler.h"
#include "cpucycles.h"
#include "tests.h"

static sampler_t sampler;

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

int main(void){
  uint32_t i, uval;
  int32_t val;
  
  if (! sampler_init(&sampler, 271, 22, 128, seed)) {
    fprintf(stderr, "error initializing sampler\n");
    return EXIT_FAILURE;
  }

  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    if (! sampler_pos_binary(&sampler, &uval)) {
      fprintf(stderr, "sampler_pos_binary failed (after %"PRIu32" iterations)\n", i);
      return EXIT_FAILURE;
    }
  }
  print_results("sampler_pos_binary: ", t, NTESTS);

  for(i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    if (! sampler_gauss(&sampler, &val)) {
      fprintf(stderr, "sampler_gauss failed (after %"PRIu32" iterations)\n", i);
      return EXIT_FAILURE;
    }
  }
  print_results("sampler_gauss: ", t, NTESTS);

  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    uval = sampler_pos_binary2(&sampler);
  }
  print_results("sampler_pos_binary2: ", t, NTESTS);

  for(i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    val = sampler_gauss2(&sampler);
  }
  print_results("sampler_gauss2: ", t, NTESTS);

  fprintf(stderr, "sampler OK\n");

  return EXIT_SUCCESS;
}
