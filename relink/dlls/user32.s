.intel_syntax noprefix
.include "macros.i"

.global MessageBoxA
MessageBoxA: die MessageBoxA
    push ebp
    mov	ebp, esp
    push [ebp + 4 + 4 * 2]
    push [ebp + 4 + 4 * 3]
    push offset 1f
    call printf
    mov	eax, 1
    leave
    ret	4 * 4
1:
    .asciz "%s: %s\n"
