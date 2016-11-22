#ifndef __BLISS_B_H__
#define __BLISS_B_H__

#include <stdint.h>

#define BLISS_B_CRYPTO_SECRETKEYBYTES 256

#define BLISS_B_CRYPTO_PUBLICKEYBYTES 85

#define BLISS_B_CRYPTO_BYTES 128

/* 
 * Generates a public key and a secret key.  The function returns 0 on
 * success, and a negative error code otherwise.
 */
extern int32_t bliss_b_crypto_sign_keypair(uint8_t *pk, uint8_t *sk);



/* 
 * Given a secret key and a message, computes the signed message.  The
 * function returns 0 on success, and a negative error code
 * otherwise.
 */
extern int32_t bliss_b_crypto_sign(uint8_t *sm, uint64_t *smlen,
				   const uint8_t *m, uint64_t mlen,
				   const uint8_t *sk);



/* 
 * Given the public key, and a signed message, checks the validity of
 * the signature, and if successful produces the original message.
 * The function returns 0 on success, -1 on failure, and a negative
 * error code, different from -1, otherwise.
 */

extern int32_t crypto_sign_open(uint8_t *m, uint64_t *mlen,
				const uint8_t *sm, uint64_t smlen,
				const uint8_t *pk);





#endif
