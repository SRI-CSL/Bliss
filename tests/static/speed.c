#include <stdlib.h>
#include <stdio.h>

#include "poly.h"
#include "cpucycles.h"
#include "tests.h"
#include "bliss_b_params.h"
#include "ntt_blzzd.h"

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
    poly_ntt_harvey(&sk_a);
  }
  print_results("poly_ntt_harvey: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    poly_invntt_harvey(&sk_a);
  }
  print_results("poly_invntt_harvey: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    poly_getnoise(&sk_a,seed,0);
  }
  print_results("poly_getnoise: ", t, NTESTS);


  const bliss_param_t *p;

  p = &bliss_b_params[BLISS_B_1];

  int32_t blzzd_a[512];

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft(blzzd_a, p->n, p->q, p->w);
  }
  print_results("ntt32_fft: ", t, NTESTS);

  return 0;
}
