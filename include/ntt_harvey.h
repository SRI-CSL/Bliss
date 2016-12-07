#ifndef _HARVEY_NTT
#define _HARVEY_NTT

#include <stddef.h>
#include <stdint.h>

#include "params.h"

void ntt_harvey(uint16_t* x, const uint16_t* wtab, const uint16_t* wtab_shoup);

// Helper function
void mul_pointwise_shoup(uint16_t* c, const uint16_t* a, const uint16_t* b,
                         const uint16_t* b_shoup);

// Constants
extern const uint16_t harvey_omegas[];
extern const uint16_t harvey_omegas_shoup[];
extern const uint16_t harvey_omegas_inv[];
extern const uint16_t harvey_omegas_inv_shoup[];
extern const uint16_t harvey_psis[];
extern const uint16_t harvey_psis_shoup[];
extern const uint16_t harvey_psis_inv[];
extern const uint16_t harvey_psis_inv_shoup[];

#endif