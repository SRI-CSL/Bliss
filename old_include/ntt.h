#ifndef NTT_H
#define NTT_H

#include <stdint.h>


void bitrev_vector(uint16_t* poly);
void mul_coefficients(uint16_t* poly, const uint16_t* factors);
void ntt(uint16_t* poly, const uint16_t* omegas);

#endif
