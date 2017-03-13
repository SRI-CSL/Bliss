#include <assert.h>
#include <stdlib.h> // for NULL

#include "shake128.h"
#include "entropy.h"


/*
 * Increment the seed
 * (we treat it as an array of bytes/little-endian)
 */
static void increment_seed(entropy_t *entropy) {
  uint32_t i;

  for (i=0; i<SHA3_512_DIGEST_LENGTH; i++) {
    entropy->seed[i] ++;
    if (entropy->seed[i] > 0) {
      break;
    }
  }
}

/*
 * Store random bits into the hash array.
 * Then increment the entropy->seed.
 *
 * - hash must be an array of n * SHA3_512_DIGEST_LENGTH bytes 
 *   (i.e., n * 64 bytes)
 */
static void refresh(entropy_t *entropy, uint8_t *hash, uint32_t n) {
  assert(n > 0);

  while (n > 0) {
    sha3_512(hash, entropy->seed, SHA3_512_DIGEST_LENGTH);
    increment_seed(entropy);
    hash += SHA3_512_DIGEST_LENGTH;
    n --;
  }
}

static void char_pool_refresh(entropy_t *entropy) {
  refresh(entropy, entropy->char_pool, EPOOL_HASH_COUNT);
  entropy->char_index = 0;
}

static void int16_pool_refresh(entropy_t *entropy) {
  refresh(entropy, (uint8_t *) entropy->int16_pool, EPOOL_HASH_COUNT);
  entropy->int16_index = 0;
}

static void int64_pool_refresh(entropy_t *entropy) {
  refresh(entropy, (uint8_t *) entropy->int64_pool, EPOOL_HASH_COUNT);
  entropy->int64_index = 0;
}



/*
 * Random 64bit integer
 */
uint64_t entropy_random_uint64(entropy_t *entropy){
  assert(entropy != NULL);

  if (entropy->int64_index >= HASH_LEN_UINT64 * EPOOL_HASH_COUNT) {
    int64_pool_refresh(entropy);
  }
  assert(entropy->int64_index < HASH_LEN_UINT64 * EPOOL_HASH_COUNT);
  return entropy->int64_pool[entropy->int64_index++];
}

/*
 * Random 16bit integer
 */
uint16_t entropy_random_uint16(entropy_t *entropy){
  assert(entropy != NULL);

  if (entropy->int16_index >= HASH_LEN_UINT16 * EPOOL_HASH_COUNT) {
    int16_pool_refresh(entropy);
  }
  assert(entropy->int16_index < HASH_LEN_UINT16 * EPOOL_HASH_COUNT);
  return entropy->int16_pool[entropy->int16_index++];
}


/*
 * Random byte
 */
uint8_t entropy_random_uint8(entropy_t *entropy){
  assert(entropy != NULL);

  if (entropy->char_index >= SHA3_512_DIGEST_LENGTH * EPOOL_HASH_COUNT) {
    char_pool_refresh(entropy);
  }
  assert(entropy->char_index < SHA3_512_DIGEST_LENGTH * EPOOL_HASH_COUNT);
  return entropy->char_pool[entropy->char_index++];
}


/*
 * Use previous function to refresh bit pool
 */
static void bit_pool_refresh(entropy_t *entropy) {
  entropy->bit_pool = entropy_random_uint64(entropy);
  entropy->bit_index = 0;
}

/*
 * Get a random bit
 */
bool entropy_random_bit(entropy_t *entropy) {
  bool bit;

  assert(entropy != NULL);

  if (entropy->bit_index >= 64) {
    bit_pool_refresh(entropy);
  }
  bit = entropy->bit_pool & 1;
  entropy->bit_pool >>= 1;
  entropy->bit_index ++;

  return bit;
}


/*
 * Return n random bits
 * - n must be no more than 32
 * - the n bits are low-order bits of the returned integer.
 */
uint32_t entropy_random_bits(entropy_t* entropy, uint32_t n) {
  uint32_t retval;

  assert(entropy != NULL && n <= 32);

  retval = 0;
  while (n  > 0) {
    retval <<= 1;
    retval |= entropy_random_bit(entropy);
    n --;
  }

  return retval;
}


/*
 * Initialize: with the given seed
 * - seed must be an array of SHA3_512_DIGEST_LENGTH bytes
 */
void entropy_init(entropy_t *entropy, const uint8_t *seed) {
  uint32_t i;

  for (i=0; i<SHA3_512_DIGEST_LENGTH; i++) {
    entropy->seed[i] = seed[i];
  }
  char_pool_refresh(entropy);
  int16_pool_refresh(entropy);
  int64_pool_refresh(entropy);
  bit_pool_refresh(entropy);
}

