#ifndef __BLISS_B_KEYS_H__
#define __BLISS_B_KEYS_H__

#include <stdint.h>
#include "bliss_b_params.h"
#include "entropy.h"

/*
 * Bliss-b private key
 *
 * The only reason we do not declare s1,s2, and a to be [512] arrays
 * is that down the track we may need to beef n up to say 1024 and beyond.
 * so this way we are flexible, and stay less committed to a fixed n.
 *
 * For Bliss-B0, n is 256.
 * For Bliss-B1 to 4, n is 512.
 */
typedef struct {
  bliss_kind_t kind;                 /* Bliss variant             */
  uint32_t n;                        /* size of arrays s1, s2, a  */
  int32_t *s1;                       /* sparse polynomial s1      */
  int32_t *s2;                       /* sparse polynomial s2      */
  int32_t *a;                        /* NTT of s1/s2              */
} bliss_private_key_t;

/*
 * Bliss-b public key
 */
typedef struct {
  bliss_kind_t kind;                /* Bliss variant              */
  uint32_t n;                       /* key size = size of array a */
  int32_t *a;                       /* NTT of s1/s2               */
} bliss_public_key_t;



/*
 * Allocates memory for the private key, and generates a new private key.
 * - private_key: structure to store the result.
 * - kind: Bliss-B variant
 * - entropy: our source of randomness, an initialized entropy object.
 *
 * Returns BLISS_B_NO_ERROR (i.e. 0) on success.
 * - in this case, the sign key is stored in private_key->s1 and private_key->s2
 *   and the public key is stored in private_key->a.
 * - s1 and s2 are stored as arrays of coefficients
 * - a is in NTT form
 *
 * Returns a negative error code is something goes wrong:
 * - BLISS_B_BAD_ARGS: kind is not supported
 * - BLISS_B_NO_MEM: failed to allocate buffers
 * - BLISS_B_RETRY: failed to construct an invertible polynomial for private_key->s1
 *   (currently, the code tries 10 times at most)
 *
 * If the returned code is negative, then private_key->s1, s2, a are all NULL.
 */
extern int32_t bliss_b_private_key_gen(bliss_private_key_t *private_key, bliss_kind_t kind, entropy_t *entropy);

/*
 * Delete the memory associated with the private_key
 * - this also zeroes out the keys
 * - this can be called if private_key->s1, s2, and a are all NULL,
 *   in which case the function does nothing.
 */
extern void bliss_b_private_key_delete(bliss_private_key_t *private_key);


/*
 * Extract the public key (polynomial a) from private_key and store it in public key.
 * - return BLISS_B_NO_ERROR if this works
 * - return BLISS_B_NO_MEM if we can't allocate array public_key->a to store the key.
 *
 * If the allocation fails, public_key->a is set to NULL.
 */
extern int32_t bliss_b_public_key_extract(bliss_public_key_t *public_key, const bliss_private_key_t *private_key);


/*
 * Delete the public_key: free memory
 * - this is safe to call if public_key->a is NULL
 */
extern void bliss_b_public_key_delete(bliss_public_key_t *public_key);

#endif
