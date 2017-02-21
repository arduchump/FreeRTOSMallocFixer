#include <stddef.h>

struct FILE;
extern void *
malloc(size_t len);
extern void
free(void *ptr);
extern void *
calloc(size_t nmemb, size_t size);
extern void *
realloc(void *ptr, size_t len);
extern FILE *
fdevopen(int (*putFunc)(char, FILE *), int (*getFunc)(FILE *));

__attribute__ ((weak)) void *
_freeRTOSMallocFixer(size_t len)
{
  return malloc(len);
}

__attribute__ ((weak)) void
_freeRTOSFreeFixer(void *ptr)
{
  free(ptr);
}

__attribute__ ((weak)) void *
_freeRTOSCallocFixer(size_t nmemb, size_t size)
{
  return calloc(nmemb, size);
}

__attribute__ ((weak)) void *
_freeRTOSReallocFixer(void *ptr, size_t len)
{
  return realloc(ptr, len);
}

__attribute__ ((weak)) FILE *
_freeRTOSFdevopenFixer(int (*putFunc)(char, FILE *), int (*getFunc)(FILE *))
{
  return fdevopen(putFunc, getFunc);
}
