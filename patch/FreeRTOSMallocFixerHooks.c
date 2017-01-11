#include <stddef.h>

extern void * malloc(size_t len);
extern void free(void * ptr);

__attribute__ ((weak)) void *
_freeRTOSMallocFixer(size_t len) 
{
	return malloc(len);
};

__attribute__ ((weak)) void
_freeRTOSFreeFixer(void *ptr) 
{
	free(ptr);
};

