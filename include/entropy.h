#ifndef _ENTROPY_H
#define _ENTROPY_H

#include <stdint.h>
#include <stdbool.h>

#define SHA3_512_DIGEST_LENGTH 64
#define EPOOL_HASH_COUNT 10
#define HASH_LEN_UINT16  (SHA3_512_DIGEST_LENGTH/sizeof(uint16_t))
#define HASH_LEN_UINT64  (SHA3_512_DIGEST_LENGTH/sizeof(uint64_t))


/*  Based on the DDLL version */
typedef struct entropy_s {
  uint64_t   bit_pool;
  uint8_t    char_pool[SHA3_512_DIGEST_LENGTH * EPOOL_HASH_COUNT];
  uint16_t   int16_pool[HASH_LEN_UINT16 * EPOOL_HASH_COUNT];
  uint64_t   int64_pool[HASH_LEN_UINT64 * EPOOL_HASH_COUNT];
  uint8_t    seed[SHA3_512_DIGEST_LENGTH];
  uint32_t   bit_index;
  uint32_t   char_index;
  uint32_t   int16_index;
  uint32_t   int64_index;
 } entropy_t;


/*
 * Initialize using a random seed
 * - the seed must be 64 bytes
 */
extern void entropy_init(entropy_t *entropy, const uint8_t *seed);

/*
 * Get one random bit, unsigned char, or 64-bit unsigned integer
 */
extern bool entropy_random_bit(entropy_t *entropy);
extern uint8_t entropy_random_uint8(entropy_t *entropy);
extern uint16_t  entropy_random_uint16(entropy_t *entropy);
extern uint64_t  entropy_random_uint64(entropy_t *entropy);

/*
 * Return n random bits
 * - n must be no more than 32
 * - the n bits are low-order bits of the returned integer.
 */
extern uint32_t entropy_random_bits(entropy_t *entropy, uint32_t n);


#endif

