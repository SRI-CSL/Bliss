#ifndef __SHAKE128_H
#define __SHAKE128_H

#include <stdint.h>

#define SHAKE128_RATE 168
#define SHA3_256_RATE 136
#define SHA3_512_RATE  72

extern void shake128_absorb(uint64_t *s, const unsigned char *input, unsigned int inputByteLen);

extern void shake128_squeezeblocks(unsigned char *output, unsigned long long nblocks, uint64_t *s);

extern void shake128(unsigned char *output, unsigned int outputByteLen, const unsigned char *input, unsigned int inputByteLen);

extern void sha3_256(unsigned char *output, const unsigned char *input, unsigned int inputByteLen);

extern void sha3_512(unsigned char *output, const unsigned char *input, unsigned int inputByteLen);

#endif /* __SHAKE128_H */
