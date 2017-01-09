#ifndef __INCLUDED_697D94AAD61111E6AA6EA088B4D1658C
#define __INCLUDED_697D94AAD61111E6AA6EA088B4D1658C

#include <Arduino.h>

extern "C"
{
void *
_mallocFixer(size_t len);

#define MALLOC_FIXER_IMPLEMEMTATION() \
  void *malloc(size_t len) { return _mallocFixer(len); }
};

#endif // __INCLUDED_697D94AAD61111E6AA6EA088B4D1658C
