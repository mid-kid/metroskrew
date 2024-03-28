.intel_syntax noprefix
.include "dlls/macros.i"

.global RegOpenKeyExA
RegOpenKeyExA:
    push [esp + 4 * 2]
    push offset 1f
    call printf
    add esp, 4 * 2
    mov eax, 1
    ret 4 * 5

1:
    .asciz "stub: RegOpenKeyExA: %s\n"

.global RegQueryValueExA
RegQueryValueExA:
    die RegQueryValueExA

.global RegCloseKey
RegCloseKey:
    die RegCloseKey
