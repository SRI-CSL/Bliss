#ifndef _ENTROPY_H
#define _ENTROPY_H


#include <stdint.h>
#include <stdbool.h>


/* probably should be consistent with  bool XXXX(...., retval_t* retval) API rather than hodge podge */


typedef struct randomizer_s {

  bool (*get_random_bit)(void);

  uint8_t (*get_random_uint8)(void);
  
  uint32_t (*get_random_bits)(int32_t);

  uint64_t (*get_random_unint64)(void);

} randomizer_t;



typedef struct entropy_s {


} entropy_t;

extern bool entropy_init(entropy_t* entropy);

extern void entropy_delete(entropy_t* entropy);

extern bool entropy_random_bit(entropy_t* entropy, bool* rbit);

extern bool entropy_random_uint8(entropy_t* entropy, uint8_t* rchar);

extern bool entropy_random_bits(entropy_t* entropy, uint32_t num_bits, uint32_t* rbits);

extern bool entropy_uint64(entropy_t* entropy, uint64_t* rint);



#endif

