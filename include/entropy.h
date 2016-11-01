#ifndef _ENTROPY_H
#define _ENTROPY_H


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <openssl/sha.h>


/* Based on BD' suggestion. Just musings at present. */
typedef struct randomizer_s {

  bool (*get_random_bit)(bool*);

  bool (*get_random_uint8)(uint8_t*);
  
  bool (*get_random_bits)(int32_t, uint32_t*);

  bool (*get_random_unint64)(uint64_t*);  //iam: not used yet

} randomizer_t;


#define EPOOL_HASH_COUNT 10
#define HASH_LEN_UINT64  SHA512_DIGEST_LENGTH / sizeof(uint64_t)


/*  Based on the DDLL version */
typedef struct entropy_s {
  uint64_t   bit_pool;
  uint8_t    char_pool[SHA512_DIGEST_LENGTH * EPOOL_HASH_COUNT];
  uint64_t   int_pool[HASH_LEN_UINT64 * EPOOL_HASH_COUNT];
  uint8_t    seed[SHA512_DIGEST_LENGTH];
  uint32_t   bit_index;
  uint32_t   char_index;
  uint32_t   int_index;
  FILE*      fp;
 
} entropy_t;

extern bool entropy_init(entropy_t* entropy);

extern void entropy_delete(entropy_t* entropy);

extern bool entropy_random_bit(entropy_t* entropy, bool* rbit);

extern bool entropy_random_uint8(entropy_t* entropy, uint8_t* rchar);

extern bool entropy_random_bits(entropy_t* entropy, uint32_t num_bits, uint32_t* rbits);

extern bool entropy_random_uint64(entropy_t* entropy, uint64_t* rint);



#endif

