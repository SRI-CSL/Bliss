#include <assert.h>
#include "bliss_b_errors.h"
#include "bliss_b_keys.h"
#include "bliss_b_signatures.h"


/**
 * GreedySC strongswan version
 *
 * should be static once we choose one and use it.
 */
void greedy_sc_strongswan(const int32_t *s1, const int32_t *s2, int32_t n, const uint32_t *c_indices, int32_t kappa, int32_t *v1, int32_t *v2)
{
	int32_t i, k, index;
	int32_t sign;

	for (i = 0; i < n; i++)
	{
		v1[i] = v2[i] = 0;
	}
	for (k = 0; k < kappa; k++)
	{
		index = c_indices[k];
		sign = 0;
        /* \xi_i = sign(<v, si>) */
		for (i = 0; i < index; i++)
          {
			sign -= (v1[i] * s1[i - index + n] + v2[i] * s2[i - index + n]);
		}
		for (i = index; i < n; i++)
		{
			sign += (v1[i] * s1[i - index] + v2[i] * s2[i - index]);
		}
		/* v = v - \xi_i . si */
		for (i = 0; i < index; i++)
		{
			if (sign > 0)
			{
				v1[i] += s1[i - index + n];
				v2[i] += s2[i - index + n];
			}
			else
			{
				v1[i] -= s1[i - index + n];
				v2[i] -= s2[i - index + n];
			}
		}
		for (i = index; i < n; i++)
		{
			if (sign > 0)
			{
				v1[i] -= s1[i - index];
				v2[i] -= s2[i - index];
			}
			else
			{
				v1[i] += s1[i - index];
				v2[i] += s2[i - index];
			}
		}
	}
}


/*
 * GreedySC blzzd version
 *
 * should be static once we choose one and use it.
 */
void greedy_sc_blzzd(const int32_t *s1, const int32_t *s2, int32_t n,  const int32_t *c_indices, int32_t kappa, int32_t *v1, int32_t *v2)
{
    int32_t index, i, k, sign;

    for (i = 0; i < n; i++) {
        v1[i] = 0;
        v2[i] = 0;
    }

    for (k = 0; k < kappa; k++) {

        index = c_indices[k];
        sign = 0;

        for (i = 0; i < n - index; i++) {
            sign += s1[i] * v1[index + i] + s2[i] * v2[index + i];
        }
        for (i = n - index; i < n; i++) {
            sign -= s1[i] * v1[index + i - n] + s2[i] * v2[index + i - n];
        }

        if (sign > 0) {
            for (i = 0; i < n - index; i++) {
                v1[index + i] -= s1[i];
                v2[index + i] -= s2[i];
            }
            for (i = n - index; i < n; i++) {
                v1[index + i - n] += s1[i];
                v2[index + i - n] += s2[i];
            }
        } else {
            for (i = 0; i < n - index; i++) {
                v1[index + i] += s1[i];
                v2[index + i] += s2[i];
            }
            for (i = n - index; i < n; i++) {
                v1[index + i - n] -= s1[i];
                v2[index + i - n] -= s2[i];
            }
        }
    }
}






int32_t bliss_b_sign(bliss_signature_t *signature,  const bliss_private_key_t private_key, const void *msg, size_t msg_sz, entropy_t *entropy){


  return BLISS_B_NO_ERROR;
}


int32_t bliss_b_verify(bliss_signature_t *signature,  const bliss_public_key_t public_key, const void *msg, size_t msg_sz){


  return BLISS_B_NO_ERROR;
}


void bliss_signature_delete(bliss_signature_t *signature){
  //FIXME: do we need to zero here? iam guesses no.

  assert(signature != NULL);

  free(signature->z1);
  signature->z1 = NULL;

  free(signature->z2);
  signature->z2 = NULL;

  free(signature->c);
  signature->c = NULL;
}
