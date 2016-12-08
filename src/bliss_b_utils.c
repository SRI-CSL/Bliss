/* 
 *
 * https://github.com/open-quantum-safe/liboqs/issues/48
 *
 */
#if defined(WINDOWS)

#include <windows.h>

void zero_memory(void *ptr, size_t len){
    SecureZeroMemory(pnt, len);
}

#else

#include<string.h>

typedef void *(*memset_t)(void *, int, size_t);

static volatile memset_t memset_func = memset;

void zero_memory(void *ptr, size_t len){
        memset_func(ptr, 0, len);
}

#endif
