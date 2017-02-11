#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "entropy.h"
#include "polynomial.h"

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

static entropy_t entropy;
static bliss_param_t params;

int main(int argc, char* argv[]){
  int32_t *g;
  bliss_param_t *p;

  p = &params;
  // we use bliss_params to pass parameters to some ntt functions
  if (!bliss_params_init(p, BLISS_B_1)) {
    fprintf(stderr, "bug: failed to initialized bliss_b_parms\n");
    return 1;
  }


  entropy_init(&entropy, seed);


  g = calloc(p->n, sizeof(int32_t));

  if(g == NULL){
    return 1;
  }

  /* randomize g */
  uniform_poly(g, p->n, 5, 5, false, &entropy);

  fprint_poly(stderr, g, p->n);

  return 0;
}
