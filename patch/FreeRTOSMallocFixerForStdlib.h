#ifndef __FREERTOS_MALLOC_FIXER_FOR_STDLIB_INCLUDED
#define __FREERTOS_MALLOC_FIXER_FOR_STDLIB_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *
_freeRTOSMallocFixer(size_t len);

#define malloc _freeRTOSMallocFixer

void
_freeRTOSFreeFixer(void *ptr);

#define free _freeRTOSFreeFixer

void *
_freeRTOSCallocFixer(size_t nmemb, size_t size);

#define calloc _freeRTOSCallocFixer

void *
_freeRTOSReallocFixer(void *ptr, size_t len);

#define realloc _freeRTOSReallocFixer

#ifdef __cplusplus
}
#endif

#endif
