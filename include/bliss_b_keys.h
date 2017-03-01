#ifndef __BLISS_B_KEYS_H__
#define __BLISS_B_KEYS_H__

#include <stdint.h>
#include "bliss_b_params.h"
#include "entropy.h"

/*
 *  bliss-b private key
 *
 * The only reason we do not declare s1,s2, and a to be [512] arrays
 * is that down the track we may need to beef n up to say 1024 and beyond.
 * so this way we are flexible, and stay less committed to a fixed n.
 */
typedef struct {
  bliss_kind_t kind;                 /* the kind of bliss       */
  int32_t *s1;                       /* sparse polynomial s1    */
  int32_t *s2;                       /* sparse polynomial s2    */
  int32_t *a;                        /* NTT of s1/s2            */
} bliss_private_key_t;

/* bliss-b public key  */
typedef struct {
  bliss_kind_t kind;                /* the kind of bliss       */
  int32_t *a;                       /* NTT of s1/s2           */
} bliss_public_key_t;


/* PRIVATE KEY API */

/* Allocates (uninitialized space) for the private key, and generates a new private key,
 * given a particular choice of kind.
 *
 * - private_key: structure to store the result.
 * - kind: the kind describes the choice of parameters in the particular variety of bliss-b that we are using.
 * - entropy: our source of randomness, an initialized entropy object.
 *
 * Returns 0 on success, or a negative error code on failure (see bliss_b_error_t).
 */
extern int32_t bliss_b_private_key_gen(bliss_private_key_t *private_key, bliss_kind_t kind, entropy_t *entropy);

/* Delete the memory associated with the private_key */
extern void bliss_b_private_key_delete(bliss_private_key_t *private_key);


/* PUBLIC KEY API */

/* Allocates (uninitialized space) for the public key, and exports it
 * from the given generated private_key.
 * Returns 0 on success, or a negative error code on failure.
 */
extern int32_t bliss_b_public_key_extract(bliss_public_key_t *public_key, const bliss_private_key_t *private_key);


/* Delete the memory associated with the public_key */
extern void bliss_b_public_key_delete(bliss_public_key_t *public_key);

#endif
