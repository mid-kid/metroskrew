.intel_syntax noprefix
.include "dlls/macros.i"

.global GetFileVersionInfoSizeA
GetFileVersionInfoSizeA:
.ifndef NDEBUG
    push [esp + 4]
    push offset 9f
    call printf
    add esp, 4 * 2
.endif

    xor eax, eax
    ret 4 * 2

.ifndef NDEBUG
9:
    .asciz "stub: GetFileVersionInfoSizeA: %s\n"
.endif

.global GetFileVersionInfoA
GetFileVersionInfoA:
    die GetFileVersionInfoA

.global VerQueryValueA
VerQueryValueA:
    die VerQueryValueA
