#ifndef __FREERTOS_MALLOC_FIXER_FOR_STDLIB_INCLUDED
#define __FREERTOS_MALLOC_FIXER_FOR_STDLIB_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

extern void * _freeRTOSMallocFixer(size_t len);
#define malloc _freeRTOSMallocFixer
extern void _freeRTOSFreeFixer(void *ptr);
#define free _freeRTOSFreeFixer

#ifdef __cplusplus
}
#endif

#endif

