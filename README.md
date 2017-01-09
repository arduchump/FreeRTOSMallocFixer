# MallocFixer

A malloc fixer for libc of Arduino

# Usage

1\. Enable GCC multiple definitions feature

    Place a file "platform.local.txt" with content below to the path {ARDUINO_IDE}/hardware/arduino/avr (same directory with "platform.txt"):

        compiler.c.extra_flags=-z muldefs
        compiler.c.elf.extra_flags=-z muldefs
        compiler.S.extra_flags=-z muldefs
        compiler.cpp.extra_flags=-z muldefs
        compiler.ar.extra_flags=
        compiler.objcopy.eep.extra_flags=
        compiler.elf2hex.extra_flags=

    So that the complier won't complain that there have another malloc implementation inside libc.a

2\. Include these codes before setup() in your *.ino

        #include <MallocFixer.h>        
        MALLOC_FIXER_IMPLEMEMTATION();

3\. Recompile the project 