#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "entropy.h"
#include "polynomial.h"

#include "cpucycles.h"
#include "tests.h"



static entropy_t entropy;



int main(int argc, char* argv[]){
  int32_t *g;
  const bliss_param_t *p;


  p = &bliss_b_params[BLISS_B_1];

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
