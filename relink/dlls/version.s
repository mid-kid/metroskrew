.intel_syntax noprefix
.include "dlls/macros.i"

.global GetFileVersionInfoSizeA
GetFileVersionInfoSizeA:
    push [esp + 4]
    push offset 1f
    call printf
    add esp, 4 * 2
    mov eax, 0
    ret 4 * 2

1:
    .asciz "stub: GetFileVersionInfoSizeA: %s\n"

.global GetFileVersionInfoA
GetFileVersionInfoA:
    die GetFileVersionInfoA

.global VerQueryValueA
VerQueryValueA:
    die VerQueryValueA
