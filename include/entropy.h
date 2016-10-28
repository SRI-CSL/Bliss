#ifndef _ENTROPY_H
#define _ENTROPY_H


#include <stdint.h>
#include <stdbool.h>



typedef struct entropy_s {


} entropy_t;

extern bool entropy_init(entropy_t* entropy);

extern void entropy_delete(entropy_t* entropy);

extern bool entropy_random_bit(entropy_t* entropy);

extern uint8_t entropy_random_uint8(entropy_t* entropy);

extern uint64_t entropy_uint64(entropy_t* entropy);



#endif

