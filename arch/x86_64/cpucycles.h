#ifndef CPUCYCLES_H
#define CPUCYCLES_H

static inline long long cpucycles(void)
{
  unsigned long long result;
  asm volatile("rdtsc; shlq $32,%%rdx; orq %%rdx,%%rax"
    : "=a" (result) ::  "%rdx");
  return result;
}


#endif
