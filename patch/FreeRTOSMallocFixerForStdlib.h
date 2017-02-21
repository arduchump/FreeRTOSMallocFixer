#ifndef __INCLUDED_5BFBDFF2F7E111E6AA6EA088B4D1658C
#define __INCLUDED_5BFBDFF2F7E111E6AA6EA088B4D1658C

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
