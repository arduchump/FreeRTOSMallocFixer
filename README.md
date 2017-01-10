# FreeRTOSMallocFixer

A malloc fixer for libc of FreeRTOS on Arduino.

We use a trick way to solve that problem.

# Usage

1\. Open "stdlib.h" in your toolchains and add codes beow

```
extern void * _freeRTOSMallocFixer(size_t len);
#define malloc _freeRTOSMallocFixer
```

So that all sources that use malloc in your project will be replaced by _freeRTOSMallocFixer(). That function provided thread-safe malloc()

2\. Include this library in your *.ino

```
#include <FreeRTOSMallocFixer.h>        
```

3\. Recompile the project 