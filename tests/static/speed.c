#include <stdlib.h>
#include <stdio.h>

#include "poly.h"
#include "cpucycles.h"
#include "tests.h"

int main()
{
  poly sk_a;
  unsigned char seed[BLISS_SEEDBYTES];
  int i;

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    poly_ntt(&sk_a);
  }
  print_results("poly_ntt: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    poly_invntt(&sk_a);
  }
  print_results("poly_invntt: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    poly_getnoise(&sk_a,seed,0);
  }
  print_results("poly_getnoise: ", t, NTESTS);

  return 0;
}
