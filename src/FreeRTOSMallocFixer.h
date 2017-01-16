#ifndef __INCLUDED_697D94AAD61111E6AA6EA088B4D1658C
#define __INCLUDED_697D94AAD61111E6AA6EA088B4D1658C

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

void *
_freeRTOSMallocFixer(size_t len);

void
_freeRTOSFreeFixer(void *ptr);

void *
_freeRTOSCallocFixer(size_t nmemb, size_t size);
void *
_freeRTOSReallocFixer(void *ptr, size_t len);

#ifdef __cplusplus
};
#endif

#endif // __INCLUDED_697D94AAD61111E6AA6EA088B4D1658C
