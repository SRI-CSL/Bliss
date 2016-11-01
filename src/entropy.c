#include <stdlib.h>

#include "entropy.h"


static void sha512(entropy_t* entropy, uint8_t* hash)
{
  uint32_t i, j;
  SHA512_CTX sha512;
  SHA512_Init(&sha512);
  SHA512_Update(&sha512, entropy->seed, SHA512_DIGEST_LENGTH);
  SHA512_Final(hash, &sha512);
  
  //iam: what is this about?
  for (i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
      if (entropy->seed[i] != 255)
    	{
	  for (j = 0; j < i; j++) entropy->seed[j] = 0;
	  entropy->seed[i]++;
	  break;
    	}
    }
}



static void bit_pool_refresh(entropy_t* entropy){
  if(entropy != NULL){
    entropy_random_uint64(entropy, &entropy->bit_pool);
    entropy->bit_index = 0;
  }
}

static void char_pool_refresh(entropy_t* entropy){
  if(entropy != NULL){
    uint32_t i;
    uint8_t *hash = (uint8_t *)(entropy->char_pool);
    for (i = 0; i < epool_hash_count; i++){
      sha512(entropy, hash);
      hash += hash_len_uint8;
    }
    entropy->char_index = 0;
  }
}

static void int_pool_refresh(entropy_t* entropy){
  if(entropy != NULL){
    uint32_t i;
    uint8_t *hash = (uint8_t *)(entropy->int_pool);
    for (i = 0; i < epool_hash_count; i++){
      sha512(entropy, hash);
      hash += hash_len_uint8;
    }
    entropy->int_index = 0;
  }
}


bool entropy_init(entropy_t* entropy){
  if(entropy != NULL){
    //iam: how portable is this?
    entropy->fp = fopen("/dev/urandom", "r");
    if(entropy->fp == NULL){ return false; }
    if(fread(entropy->seed, sizeof(uint8_t), hash_len_uint8, entropy->fp) != hash_len_uint8){
      entropy_delete(entropy);
      return false; 
    }
    char_pool_refresh(entropy);
    int_pool_refresh(entropy);
    bit_pool_refresh(entropy);
    return true;
  }
  return false;
}





void entropy_delete(entropy_t* entropy){
  if(entropy != NULL){
    if(entropy->fp != NULL){
      fclose(entropy->fp);
      entropy->fp = NULL;
    }
  }
  return;
}

extern bool entropy_random_bit(entropy_t* entropy, bool* rbit){
  if(entropy != NULL && rbit != NULL){
    if(entropy->bit_index >= 64){
      bit_pool_refresh(entropy);
    }
    *rbit = entropy->bit_pool;
    entropy->bit_pool >>= 1;
    return true;
  }
  return false;
}

extern bool entropy_random_uint8(entropy_t* entropy, uint8_t* rchar){
  if(entropy != NULL){
    if(entropy->char_index >= hash_len_uint8 * epool_hash_count){
      char_pool_refresh(entropy);
    }
    *rchar = entropy->int_pool[entropy->char_index++];
    return true;
  }
  return false;
}

extern bool entropy_random_bits(entropy_t* entropy, uint32_t num_bits, uint32_t* rbits){
  if(entropy != NULL && num_bits <= 32){
    uint32_t retval = 0;

    while(num_bits > 0){

      if(entropy->bit_index >= 64){
	bit_pool_refresh(entropy);
      }

      retval <<= 1;
      retval |= entropy->bit_pool;
      entropy->bit_pool >>= 1;
      num_bits--;
    }
    *rbits = retval;
    
    return true;
  }
  return false;
}

extern bool entropy_random_uint64(entropy_t* entropy, uint64_t* rint){
  if(entropy != NULL){
    if(entropy->int_index >= hash_len_uint64 * epool_hash_count){
      int_pool_refresh(entropy);
    }
    *rint = entropy->int_pool[entropy->int_index++];
    return true;
  }
  return false;
}
