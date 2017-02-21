# FreeRTOSMallocFixer

A malloc fixer for libc of FreeRTOS on Arduino.

We use a trick way to solve that problem.

# Usage

1\. Put "patch/FreeRTOSMallocFixerForStdlib.h" and "FreeRTOSMallocFixerForStdio.h" to the same folder of "stdlib.h" in your toolchains

2\. Include "FreeRTOSMallocFixerForStdlib.h" at the end of "stdlib.h"

```
#include "FreeRTOSMallocFixerForStdlib.h"
```

3\. Include "FreeRTOSMallocFixerForStdio.h" at the end of "stdio.h"

```
#include "FreeRTOSMallocFixerForStdio.h"
```

So that all sources that use malloc in your project will be replaced by _freeRTOSMallocFixer(). That function provided thread-safe malloc()

3\. Put "patch/FreeRTOSMallocFixerHooks.c" to `"{ARDUINO_IDE}/hardware/arduino/avr/cores/arduino"`

4\. Include this library in your *.ino to really take effect (otherwise it will use the origin malloc() and free()

```
#include <FreeRTOSMallocFixer.h>        
```

5\. Recompile the project 
