#include <stdlib.h>
#include <stdio.h>

#include "sampler.h"
#include "cpucycles.h"
#include "tests.h"


static sampler_t sampler;

int main(void){
  int i;
  uint32_t val;
  
  if(!sampler_init(&sampler, 271, 22, 128)){
    fprintf(stderr, "error initializing sampler\n");
    return EXIT_FAILURE;
  }

  
  for(i=0; i<NTESTS; i++)
    {
      t[i] = cpucycles();
      sampler_pos_binary(&sampler, &val);
    }
  print_results("sampler_pos_binary: ", t, NTESTS);



  

  for(i=0; i<NTESTS; i++)
    {
      t[i] = cpucycles();
      fprintf(stderr, "%d\n", i);
      sampler_gauss(&sampler, &val);
    }
  print_results("sampler_gauss: ", t, NTESTS);

  
  sampler_delete(&sampler);
  fprintf(stderr, "sampler OK\n");
  return EXIT_SUCCESS;
}
