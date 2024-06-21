# vim:set ft=asm:

# Portability macros

.macro func name
.ifndef _WIN32
.global \name
\name:
.else
.global _\name
_\name:
.endif
.endm

.macro wcall name
.ifndef _WIN32
    call \name
.else
    call _\name
.endif
.endm

.macro wjmp name
.ifndef _WIN32
    jmp \name
.else
    jmp _\name
.endif
.endm
