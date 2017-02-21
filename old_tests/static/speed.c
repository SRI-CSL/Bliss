#include <stdlib.h>
#include <stdio.h>

#include "poly.h"
#include "cpucycles.h"
#include "tests.h"
#include "bliss_b_params.h"
#include "blzzd1024_tables.h"
#include "ntt_blzzd.h"
#include "ntt_microsoft.h"

int main()
{
  bliss_param_t p;
  poly sk_a;
  unsigned char seed[BLISS_SEEDBYTES];
  int i;

  // we use bliss_params to pass parameters to some ntt functions
  if (!bliss_params_init(&p, BLISS_B_1)) {
    fprintf(stderr, "bug: failed to initialized bliss_b_parms\n");
    return 1;
  }


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

  int32_t blzzd_a[512];

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft(blzzd_a, p.n, p.q, p.w);
  }
  print_results("ntt32_fft (512): ", t, NTESTS);

  int32_t blzzd_b[1024];
  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft(blzzd_b, 1024, 12289, w12289n1024);
  }
  print_results("ntt32_fft (1024): ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft1024_var1(blzzd_b, 12289, w12289n1024);
  }
  print_results("ntt32_fft1024_var1: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft1024_var2(blzzd_b, 12289, w12289n1024);
  }
  print_results("ntt32_fft1024_var2: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft1024_var3(blzzd_b, 12289, w12289n1024);
  }
  print_results("ntt32_fft1024_var3: ", t, NTESTS);

  for(i=0; i<NTESTS; i++)
  {
    t[i] = cpucycles();
    ntt32_fft1024_var4(blzzd_b, 12289, w12289n1024);
  }
  print_results("ntt32_fft1024_var4: ", t, NTESTS);

  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    NTT_CT_std2rev_12289(blzzd_b, psi_rev_ntt1024_12289, 1024);
  }
  print_results("ntt_microsoft (1024): ", t, NTESTS);

  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    INTT_GS_rev2std_12289(blzzd_b, omegainv_rev_ntt1024_12289,
			  omegainv7N_rev_ntt1024_12289, Ninv8_ntt1024_12289, 1024);
  }
  print_results("inv_ntt_microsoft (1024): ", t, NTESTS);

  return 0;
}
