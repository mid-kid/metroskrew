.intel_syntax noprefix
.include "macros.i"

.global RegOpenKeyExA
RegOpenKeyExA:
.ifndef NDEBUG
    push [esp + 4 * 2]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

    mov eax, 1
    ret 4 * 5

.ifndef NDEBUG
9:
    .asciz "stub: RegOpenKeyExA: %s\n"
.endif

.global RegQueryValueExA
RegQueryValueExA:
    die RegQueryValueExA

.global RegCloseKey
RegCloseKey:
    die RegCloseKey
