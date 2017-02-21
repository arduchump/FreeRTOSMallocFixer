#ifndef __INCLUDED_4C8C2DE2F7E111E6AA6EA088B4D1658C
#define __INCLUDED_4C8C2DE2F7E111E6AA6EA088B4D1658C

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE *
_freeRTOSFdevopenFixer(int (*putFunc)(char, FILE *), int (*getFunc)(FILE *));

#define fdevopen _freeRTOSFdevopenFixer

#ifdef __cplusplus
}
#endif

#endif
