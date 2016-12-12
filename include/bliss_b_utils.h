#ifndef _BLISS_B_UTILS_H
#define _BLISS_B_UTILS_H

/* 
 *  Zeros len bytes of ptr, designed in such a way as to NOT be
 *  optimized out by compilers.
 *   - ptr, pointer to bytes to be zeroed.
 *   - len, the number of bytes to be zeroed.
 */
extern void zero_memory(void *ptr, size_t len);

#endif
