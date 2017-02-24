#ifndef CPUCYCLES_H
#define CPUCYCLES_H

// for i686
static inline long long cpucycles(void)
{
  unsigned long long result;
  //  asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
  //    : "=a" (result) ::  "%rdx");
  asm volatile ( "rdtsc" : "=A"(result) );
  return result;
}


#endif
