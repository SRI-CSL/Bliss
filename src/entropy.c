#include <stdlib.h>

#include "entropy.h"


bool entropy_init(entropy_t* entropy){
  return true;
}

void entropy_delete(entropy_t* entropy){
  return;
}

extern bool entropy_random_bit(entropy_t* entropy, bool* rbit){
  if(entropy != NULL && rbit != NULL){
    *rbit = 0;

    return true;
  }
  return false;
}

extern bool entropy_random_uint8(entropy_t* entropy, uint8_t* rchar){
  if(entropy != NULL){
    *rchar = 0;
    
    return true;
  }
  return false;
}

extern bool entropy_random_bits(entropy_t* entropy, uint32_t num_bits, uint32_t* rbits){
  if(entropy != NULL){
    *rbits = 0;
    
    return true;
  }
  return false;
}

extern bool entropy_random_uint64(entropy_t* entropy, uint64_t* rint){
  if(entropy != NULL){
    *rint = 0;
    
    return true;
  }
  return false;
}
