#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

extern uint16_t montgomery_reduce(uint32_t a);

extern uint16_t barrett_reduce(uint16_t a);

#endif
