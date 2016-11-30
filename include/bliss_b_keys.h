#ifndef __BLISS_B_KEYS_H__
#define __BLISS_B_KEYS_H__

#include <stdint.h>
#include <stdbool.h>

/* bliss-b private key */
typedef struct {
  int32_t kind;                      /* index of parameter set */
  int32_t *f;                        /* sparse polynomial f    */
  int32_t *g;                        /* sparse polynomial g    */
  int32_t *a;                        /* NTT of f/g             */
} bliss_private_key_t;

/* bliss-b public key  */
typedef struct {
  int32_t kind;                     /* index of parameter set */
  int32_t *a;                       /* NTT of f/g             */
} bliss_public_key_t;


/* PRIVATE KEY API */

/* Delete the memory associated with the private_key */
void bliss_b_private_key_delete(bliss_private_key_t *private_key);

/* Allocate (uninitialized space) for the private key. 
 * Sets the kind field on the key to the given value (if valid?).
 * Returns true on success, or false on failure (and sets errno to the appropriate value)
 * (IAM2BD: do we want to return an int code instead? like the API?)
 */
bool bliss_b_private_key_init(bliss_private_key_t *private_key, int32_t kind);


/* Generate a new private key. The private_key should already be initialized.
 * In particular it should have a valid kind set.
 * (IAM2BD: Do we want to roll init and gen into one function)
 */
bool bliss_b_private_key_gen(bliss_private_key_t *private_key);


/* PUBLIC KEY API */

/* Delete the memory associated with the public_key */
void bliss_b_public_key_delete(bliss_public_key_t *public_key);

/* Allocate (uninitialized space) for the public key. 
 * Sets the kind field on the key to the given value (if valid?).
 * Returns true on success, or false on failure (and sets errno to the appropriate value)
 * (IAM2BD: do we want to return an int code instead? like the API?)
 */
bool bliss_b_public_key_init(bliss_public_key_t *public_key, int32_t kind);


/* Generate a new public key from the given generated private_key. 
 * The public_key should already be initialized.
 * In particular it should have a valid kind set, and that kind should match the
 * private key kind.
 * (IAM2BD: Do we want to roll init and gen into one function)
 */
bool bliss_b_public_key_gen(bliss_private_key_t *private_key, bliss_public_key_t *public_key);






#endif

