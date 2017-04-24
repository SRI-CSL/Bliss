#include <stdio.h>
#include <sys/time.h>

#define NTESTS 1024


#if defined(WINDOWS)
#include <Windows.h>

// From github.com/open-quantum-safe/liboqs
// (under MIT License)
int gettimeofday(struct timeval *tp, struct timezone *tzp) {
  // Note: some broken versions only have 8 trailing zero's, the correct epoch
  // has 9 trailing zero's
  static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

  SYSTEMTIME system_time;
  FILETIME file_time;
  uint64_t time;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time, &file_time);
  time = ((uint64_t)file_time.dwLowDateTime);
  time += ((uint64_t)file_time.dwHighDateTime) << 32;

  tp->tv_sec = (long)((time - EPOCH) / 10000000L);
  tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
  return 0;
}
#endif

double average_t(double *t, size_t tlen) {
  double acc = 0.0;
  size_t i;
  for (i = 0; i < tlen; i++) acc += t[i] / (tlen);
  return acc;
}
